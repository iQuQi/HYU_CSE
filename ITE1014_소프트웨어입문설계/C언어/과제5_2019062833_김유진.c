#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

//프린트함수 생성
void print(int ** arr,int sero, int garo) {
	for (int i = 0; i < sero; i++) {
		for (int j = 0; j < garo; j++) {
			printf("%4d ", arr[i][j]);
		}
		printf("\n");
	}
}
//회전해주는 함수 생성하기
int** rotate(int ** arr) {
	int i, j,garo,sero;
	int** returnArr=NULL;
	
	//매개변수로 받아온 배열을 가로크기와 세로크기를 구해준다
	sero = _msize(arr)/sizeof(arr[0]);
	garo = _msize(arr[0]) / sizeof(arr[0][0]);

	//이차원 동적 할당
	returnArr = (int **)malloc(garo * sizeof(int*));

	//일차원 각각 동적할당
	for (i = 0; i < garo; ++i) {
		returnArr[i] = (int *)malloc(sero * sizeof(int));
	}
	//받아온배열의 값들을 새로만들어준 배열에 90도 회전한 상태로 넣어준다.
	for (i = 0; i < garo; ++i) {
		for (j = 0; j < sero; ++j) {
			returnArr[i][j] = arr[sero -1- j][i];
		}
	}
	//일차원 각각 메모리 해제
	for (i = 0; i < sero; ++i) {
		free(arr[i]);
	}

	//이차원 메모리 해제
	free(arr);

	//회전시킨 배열을 리턴해준다
	return returnArr;
}

int main() {
	int garo, sero,i,j;
	int** arr = NULL;

	//사용자에게 가로와 세로크기를 입력받는다
	printf("input row size of the matrix : ");
	scanf("%d", &sero);
	printf("input column size of the matrix : ");
	scanf("%d", &garo);

	//이차원 동적 할당
	arr = (int **)malloc(sero*sizeof(int *));

	//일차원 각각동적 할당
	for (i = 0; i < sero; ++i) {
		arr[i] = (int *)malloc(garo * sizeof(int));

		//값을 넣어준다
		for (j = 0; j < garo; ++j) {
			if (i == 0 && j == 0) {
				arr[i][j] = 1;
			}
			else if(j!=0){
				arr[i][j] = arr[i][j - 1]+1;
			}
			else {
				arr[i][j] = arr[i-1][garo-1]+1;
			}
			
		}
	}
	//0도 배열프린트
	printf("< degree : 0 >\n");
	print(arr, sero, garo);
	
	//90도 회전배열프린트
	printf("< degree : 90 >\n");
	arr = rotate(arr);
	print(arr, garo, sero);
	
	//180도 회전 배열 프린트
	printf("< degree : 180 >\n");
	arr = rotate(arr);
	print(arr, sero, garo);
	
	//270도 회전 배열 프린트
	printf("< degree : 270 >\n");
	arr = rotate(arr);
	print(arr, garo, sero);

	
	
	







}