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

	//���� ����� 0�̶�� ����ó���� �θ���
	if (size <= 0)
		return NULL;

	//����� ������������ ���� ũ�ٸ� 
	if (size > DSIZE) reSize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
	//�۰ų� ���ٸ� reSize�� ����������� 2��
	else reSize = 2 * DSIZE;

	//��������Ʈ���� �� �ڸ��� ã�Ƽ� �ִ´�.
	if ((bp = find_fit(reSize)) != NULL) {
		place(bp, reSize);
		return bp;
	}

	//�� �ڸ��� ���ٸ� ������ Ȯ��
	newSize = MAX(reSize, CHUNKSIZE);
	//Ȯ���� �����ߴٸ� null����
	if ((bp = extend_heap(newSize / WSIZE)) == NULL) return NULL;
	//�ƴ϶�� ���ڸ��� reSize��ŭ alloc
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
	//������� �޾ƿ´�
	size_t size = GET_SIZE(HDRP(bp));
	//����� ǲ�͸� �������ش�.
	put_head_foot(bp, size, 0);
	//������ ���� üũ
	put(bp, 0);

	//��ģ ����� ����Ʈ�� �ִ´�.
	free_list_insert(coalesce(bp));

}

/* $end mmfree */

/*
 * mm_realloc - naive implementation of mm_realloc
 */
void *mm_realloc(void *oldbp, size_t size)
{
	//���� ������
	size_t oldSize = GET_SIZE(HDRP(oldbp));
	size_t reSize, blockNext, tmp, _size, sizeCopy;
	//�� ������ ����ų ������
	void *newbp;


	//���� �������͸� �޾Ҵٸ� �׳� �� ������ ��ŭ malloc
	if (oldbp == NULL) {
		return mm_malloc(size);
	}

	//����� 0�̶�� free�� ����
	if (size == 0) {
		mm_free(oldbp);
		return NULL;
	}

	//��������� ���� �۰ų� ������ 
	if (size <= DSIZE) {
		reSize = 2 * DSIZE;
	}
	//���������� ũ�ٸ� 
	else {
		reSize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
	}

	//���� ����� ������� �ʾҴٸ� �߰� �۾��ʿ� x ������ ��ȯ
	if (reSize == oldSize) {
		return oldbp;
	}

	//�� ū �������� �� �Ҵ� �ϴ� ���
	else if (reSize > oldSize) {
		//������ ����� ���ļ� �ذᰡ������ Ȯ���Ѵ�
		blockNext = GET_SIZE(HDRP(NEXT_BLKP(oldbp)));
		//���� ����� �Ҵ��� �ȵ� free ����̰� ���Ҵ��� ������ ũ���� ���
		if (!GET_ALLOC(HDRP(NEXT_BLKP(oldbp)))) {
			if (blockNext + oldSize >= reSize) {
				//�����ϴٸ� ���� ���� ��������Ʈ���� ���� ��Ű�� ��� ǲ�� ���� ����
				free_list_remove(NEXT_BLKP(oldbp));
				put_head_foot(oldbp, oldSize + blockNext, 1);
				return oldbp;
			}
		}

		//���ʺ���� free�� ��쿡�� ���Ҵ�� ��� ������ �����ؾ��ϹǷ�
		//���ǹ� �ϹǷ� �׳� ���� ����� �������ְ� ������� �Ҵ��ϸ� ������ �����Ѵ�.

		//���ο� ������ �� �Ҵ�
		newbp = mm_malloc(size);
		//���� ���� : ������ ũ��= ����ũ�⿡�� �����ǲ�ͻ���ŭ
		sizeCopy = oldSize - DSIZE;
		memcpy(newbp, oldbp, sizeCopy);
		//���� ��� free
		mm_free(oldbp);
		return newbp;
	}

	//�� ���� �������� ���Ҵ� �ϴ� ���
	else {
		//�پ���� �� ũ�Ⱑ ���������� ũ�ų� �������
		if (oldSize - reSize >= BLOCKSIZE) {
			//�پ�� ������� ���� ����
			put_head_foot(oldbp, reSize, 1);
			//next �� ������ �޾ƿͼ� ���ǲ�� ���� ���� �� free ����Ʈ�� �־��ش�.
			newbp = NEXT_BLKP(oldbp);
			put_head_foot(newbp, oldSize - reSize, 0);
			put(newbp, 0);
			free_list_insert(newbp);
		}

		//�پ�� ũ�Ⱑ ���������� ���� ���� �׳� ����
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




//����� ǲ���� ��������
static void put_head_foot(void *bp, size_t size, int t) {
	PUT(HDRP(bp), PACK(size, t));
	PUT(FTRP(bp), PACK(size, t));

}

//t�� �ش� �ּҿ� write
static void put(void *bp, unsigned int t) {
	PUT(p_to_char(bp), t);
	PUT(pAtC(bp), t);

}
