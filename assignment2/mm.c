/*
 * mm-implicit.c -  Simple allocator based on implicit free lists,
 *                  first fit placement, and boundary tag coalescing.
 *
 * Each block has header and footer of the form:
 *
 *      31                     3  2  1  0
 *      -----------------------------------
 *     | s  s  s  s  ... s  s  s  0  0  a/f
 *      -----------------------------------
 *
 * where s are the meaningful size bits and a/f is set
 * iff the block is allocated. The list has the following form:
 *
 * begin                                                          end
 * heap                                                           heap
 *  -----------------------------------------------------------------
 * |  pad   | hdr(8:a) | ftr(8:a) | zero or more usr blks | hdr(8:a) |
 *  -----------------------------------------------------------------
 *          |       prologue      |                       | epilogue |
 *          |         block       |                       | block    |
 *
 * The allocated prologue and epilogue blocks are overhead that
 * eliminate edge conditions during coalescing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

#define WSIZE 4
#define DSIZE 8 
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y)? (x): (y))
#define MIN(x, y) ((x) > (y)? (y): (x))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *) (p))
#define PUT(p, val) (*(unsigned int *) (p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define p_to_char(bp) ((char *) (bp))
#define pAtC(bp) ((char *) (bp + WSIZE))
#define p_char_GET(bp) ((char *) GET(p_to_char(bp)))
#define pAtC_GET(bp) ((char *) GET(pAtC(bp)))


#define HDRP(bp) ((char *) (bp) - WSIZE)
#define FTRP(bp) ((char *) (bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define HDRP(oldbp) ((char *) (oldbp) - WSIZE)
#define FTRP(oldbp) ((char *) (oldbp) + GET_SIZE(HDRP(oldbp)) - DSIZE)

#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(oldbp) ((char *)(oldbp) - GET_SIZE(((char *)(oldbp) - DSIZE)))
#define NEXT_BLKP(oldbp) ((char *)(oldbp) + GET_SIZE(HDRP(oldbp)))

 /* private helper function definitions */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);


#define ClassSize 17 
#define BLOCKSIZE (4*WSIZE) 
#define eight 8
#define sideby(size) (((size) + (eight-1)) & ~0x7)
#define SIZE_T_SIZE (sideby(sizeof(size_t)))

static char *_heap_listp = 0;
static char **_free_listp = 0;
static void free_list_insert(void* bp);
static void free_list_remove(void* bp);


static void put_head_foot(void *bp, size_t size, int t);
static void put(void *bp, unsigned int t);



/*
 * mm_init - Initialize the memory manager
 */
 /* $begin mminit */
int mm_init(void)
{
	/* create the initial empty heap */
	if ((_heap_listp = mem_sbrk(WSIZE*(ClassSize + 2 + 1))) == (void *)-1)
		return -1;

	memset(_heap_listp, 0, ClassSize*WSIZE);
	_free_listp = (char **)_heap_listp;


	_heap_listp += ClassSize * WSIZE;
	PUT(_heap_listp, PACK(DSIZE, 1));
	PUT(_heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
	PUT(_heap_listp + (2 * WSIZE), PACK(0, 1));
	_heap_listp += (1 * WSIZE);
	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
	if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
		return -1;

	return 0;
}
/* $end mminit */

/*
 * mm_malloc - Allocate a block with at least size bytes of payload
 */
 /* $begin mmmalloc */
void *mm_malloc(size_t size)
{

	char *bp;
	size_t reSize;
	size_t newSize, tmp_size;

	//만약 사이즈가 0이라면 에러처리로 널리턴
	if (size <= 0)
		return NULL;

	//사이즈가 더블워드사이즈 보다 크다면 
	if (size > DSIZE) reSize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
	//작거나 같다면 reSize는 더블사이즈의 2배
	else reSize = 2 * DSIZE;

	//프리리스트에서 들어갈 자리를 찾아서 넣는다.
	if ((bp = find_fit(reSize)) != NULL) {
		place(bp, reSize);
		return bp;
	}

	//들어갈 자리가 없다면 공간을 확장
	newSize = MAX(reSize, CHUNKSIZE);
	//확장이 실패했다면 null리턴
	if ((bp = extend_heap(newSize / WSIZE)) == NULL) return NULL;
	//아니라면 그자리에 reSize만큼 alloc
	place(bp, reSize);

	return bp;
}
/* $end mmmalloc */

/*
 * mm_free - Free a block
 */
 /* $begin mmfree */
void mm_free(void *bp)
{
	//블럭사이즈를 받아온다
	size_t size = GET_SIZE(HDRP(bp));
	//헤더랑 풋터를 변경해준다.
	put_head_foot(bp, size, 0);
	//안전을 위한 체크
	put(bp, 0);

	//합친 블락을 리스트에 넣는다.
	free_list_insert(coalesce(bp));

}

/* $end mmfree */

/*
 * mm_realloc - naive implementation of mm_realloc
 */
void *mm_realloc(void *oldbp, size_t size)
{
	//원래 사이즈
	size_t oldSize = GET_SIZE(HDRP(oldbp));
	size_t reSize, blockNext, tmp, _size, sizeCopy;
	//새 공간을 가리킬 포인터
	void *newbp;


	//만약 널포인터를 받았다면 그냥 그 사이즈 만큼 malloc
	if (oldbp == NULL) {
		return mm_malloc(size);
	}

	//사이즈가 0이라면 free랑 동일
	if (size == 0) {
		mm_free(oldbp);
		return NULL;
	}

	//블락사이즈 보다 작거나 같으면 
	if (size <= DSIZE) {
		reSize = 2 * DSIZE;
	}
	//블락사이즈보다 크다면 
	else {
		reSize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
	}

	//만약 사이즈가 변경되지 않았다면 추가 작업필요 x 원래꺼 반환
	if (reSize == oldSize) {
		return oldbp;
	}

	//더 큰 공간으로 재 할당 하는 경우
	else if (reSize > oldSize) {
		//오른쪽 블락과 합쳐서 해결가능한지 확인한다
		blockNext = GET_SIZE(HDRP(NEXT_BLKP(oldbp)));
		//옆에 블락이 할당이 안된 free 블락이고 재할당이 가능한 크기인 경우
		if (!GET_ALLOC(HDRP(NEXT_BLKP(oldbp)))) {
			if (blockNext + oldSize >= reSize) {
				//가능하다면 옆의 블럭을 프리리스트에서 제외 시키고 헤더 풋터 정보 갱신
				free_list_remove(NEXT_BLKP(oldbp));
				put_head_foot(oldbp, oldSize + blockNext, 1);
				return oldbp;
			}
		}

		//왼쪽블락은 free인 경우에도 재할당시 모든 정보를 복사해야하므로
		//무의미 하므로 그냥 현재 블락을 프리해주고 새블락을 할당하며 정보를 복사한다.

		//새로운 공간에 재 할당
		newbp = mm_malloc(size);
		//정보 복사 : 복사할 크기= 원래크기에서 헤더랑풋터뺀만큼
		sizeCopy = oldSize - DSIZE;
		memcpy(newbp, oldbp, sizeCopy);
		//원래 블락 free
		mm_free(oldbp);
		return newbp;
	}

	//더 적은 공간으로 재할당 하는 경우
	else {
		//줄어들어야 할 크기가 블락사이즈보다 크거나 같은경우
		if (oldSize - reSize >= BLOCKSIZE) {
			//줄어든 사이즈로 정보 갱신
			put_head_foot(oldbp, reSize, 1);
			//next 블럭 정보를 받아와서 헤더풋터 정보 갱신 후 free 리스트에 넣어준다.
			newbp = NEXT_BLKP(oldbp);
			put_head_foot(newbp, oldSize - reSize, 0);
			put(newbp, 0);
			free_list_insert(newbp);
		}

		//줄어든 크기가 블락사이즈보다 작은 경우는 그냥 리턴
		return oldbp;
	}
}

/*
 * mm_checkheap - Check the heap for consistency
 *//*
void mm_checkheap(int verbose)
{
	char *bp = heap_listp;

	if (verbose)
	printf("Heap (%p):\n", heap_listp);

	if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
	printf("Bad prologue header\n");
	checkblock(heap_listp);

	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (verbose)
		printblock(bp);
	checkblock(bp);
	}

	if (verbose)
	printblock(bp);
	if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
	printf("Bad epilogue header\n");
}*/

/* The remaining routines are internal helper routines */

/*
 * extend_heap - Extend heap with free block and return its block pointer
 */
 /* $begin mmextendheap */
static void *extend_heap(size_t words) {
	char *bp;
	size_t size;


	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size)) == -1)
		return NULL;


	put_head_foot(bp, size, 0);
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));


	bp = coalesce(bp);

	free_list_insert(bp);

	return bp;
}
/* $end mmextendheap */

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
 /* $begin mmplace */
 /* $begin mmplace-proto */
static void place(void *bp, size_t asize) {

	size_t hSize = GET_SIZE(HDRP(bp));

	if ((hSize - asize) >= BLOCKSIZE) {

		free_list_remove(bp);
		put_head_foot(bp, asize, 1);

		bp = NEXT_BLKP(bp);
		put_head_foot(bp, hSize - asize, 0);

		put(bp, 0);

		free_list_insert(bp);
	}
	else {
		free_list_remove(bp);
		put_head_foot(bp, hSize, 1);

	}
}
/* $end mmplace */

/*
 * find_fit - Find a fit for a block with asize bytes
 */
static void *find_fit(size_t asize) {
	void *class_p, *bp;
	size_t b_size;
	size_t tmp_s = asize;
	int sizeC = 0;
	int ch = 0, sum = 0;
	while (asize > BLOCKSIZE && sizeC < ClassSize - 1) {
		sizeC++;
		ch += asize % 2;
		asize /= 2;
	}
	if (sizeC < ClassSize - 1 && ch > 0 && asize == BLOCKSIZE) {
		sizeC++;
	}


	while (sizeC < ClassSize) {
		class_p = _free_listp + sizeC;


		if (GET(class_p) != 0) {
			bp = (void *)GET(class_p);
			while (bp != ((void *)0)) {
				b_size = GET_SIZE(HDRP(bp));
				if (tmp_s <= b_size) {
					return bp;
				}
				bp = pAtC_GET(bp);
			}
		}
		sizeC++;
	}

	return NULL;
}

/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) {
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));


	if (prev_alloc && next_alloc) {
		return bp;
	}

	else if (!prev_alloc && next_alloc) {

		free_list_remove(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}

	else if (prev_alloc && !next_alloc) {

		free_list_remove(NEXT_BLKP(bp));

		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		put_head_foot(bp, size, 0);
	}



	else {

		free_list_remove(PREV_BLKP(bp));
		free_list_remove(NEXT_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		put_head_foot(bp, size, 0);
		bp = PREV_BLKP(bp);
	}

	return bp;
}
/*
static void printblock(void *bp)
{
	size_t hsize, halloc, fsize, falloc;

	hsize = GET_SIZE(HDRP(bp));
	halloc = GET_ALLOC(HDRP(bp));
	fsize = GET_SIZE(FTRP(bp));
	falloc = GET_ALLOC(FTRP(bp));

	if (hsize == 0) {
	printf("%p: EOL\n", bp);
	return;
	}

	printf("%p: header: [%d:%c] footer: [%d:%c]\n", bp,
	   hsize, (halloc ? 'a' : 'f'),
	   fsize, (falloc ? 'a' : 'f'));
}

static void checkblock(void *bp)
{
	if ((size_t)bp % 8)
	printf("Error: %p is not doubleword aligned\n", bp);
	if (GET(HDRP(bp)) != GET(FTRP(bp)))
	printf("Error: header does not match footer\n");
}*/

static void free_list_insert(void *bp) {
	size_t size = GET_SIZE(HDRP(bp));
	char **sizeC_ptr;
	unsigned int bp_val = (unsigned int)bp;

	int sizeC = 0;
	int ch = 0;
	while (size > BLOCKSIZE && sizeC < ClassSize - 1) {
		sizeC++;
		ch += size % 2;
		size /= 2;
	}
	if (sizeC < ClassSize - 1 && ch > 0 && size == BLOCKSIZE) {
		sizeC++;
	}


	sizeC_ptr = _free_listp + sizeC;
	if (GET(sizeC_ptr) == 0) {
		PUT(sizeC_ptr, bp_val);
		PUT(p_to_char(bp), (unsigned int)sizeC_ptr);
		PUT(pAtC(bp), 0);
	}

	else {
		PUT(p_to_char(bp), (unsigned int)sizeC_ptr);
		PUT(pAtC(bp), GET(sizeC_ptr));
		PUT(p_to_char(GET(sizeC_ptr)), bp_val);
		PUT(sizeC_ptr, bp_val);
	}

}


static void free_list_remove(void *bp) {
	int pre;

	unsigned int valP = (unsigned int)p_char_GET(bp);
	unsigned int p = (unsigned int)_free_listp;
	unsigned int f = p + WSIZE * (ClassSize - 1);

	if (valP > f || valP < p)
		pre = 1;
	else if ((f - valP) % WSIZE)
		pre = 1;
	else pre = 0;
	int suc = (pAtC_GET(bp) != (void *)0);

	if (GET_ALLOC(HDRP(bp))) {

		return;
	}

	if (!pre && suc) {
		PUT(p_char_GET(bp), (unsigned int)pAtC_GET(bp));
		PUT(p_to_char(pAtC_GET(bp)), (unsigned int)p_char_GET(bp));
	}



	else if (pre && suc) {
		PUT(pAtC(p_char_GET(bp)), (unsigned int)pAtC_GET(bp));
		PUT(p_to_char(pAtC_GET(bp)), (unsigned int)p_char_GET(bp));
	}

	else if (!pre && !suc) {
		PUT(p_char_GET(bp), (unsigned int)pAtC_GET(bp));
	}

	else {
		PUT(pAtC(p_char_GET(bp)), 0);
	}

	put(bp, 0);

}




//헤더와 풋터의 정보조정
static void put_head_foot(void *bp, size_t size, int t) {
	PUT(HDRP(bp), PACK(size, t));
	PUT(FTRP(bp), PACK(size, t));

}

//t로 해당 주소에 write
static void put(void *bp, unsigned int t) {
	PUT(p_to_char(bp), t);
	PUT(pAtC(bp), t);

}
