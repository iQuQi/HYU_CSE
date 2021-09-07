#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

//����Ʈ�Լ� ����
void print(int ** arr,int sero, int garo) {
	for (int i = 0; i < sero; i++) {
		for (int j = 0; j < garo; j++) {
			printf("%4d ", arr[i][j]);
		}
		printf("\n");
	}
}
//ȸ�����ִ� �Լ� �����ϱ�
int** rotate(int ** arr) {
	int i, j,garo,sero;
	int** returnArr=NULL;
	
	//�Ű������� �޾ƿ� �迭�� ����ũ��� ����ũ�⸦ �����ش�
	sero = _msize(arr)/sizeof(arr[0]);
	garo = _msize(arr[0]) / sizeof(arr[0][0]);

	//������ ���� �Ҵ�
	returnArr = (int **)malloc(garo * sizeof(int*));

	//������ ���� �����Ҵ�
	for (i = 0; i < garo; ++i) {
		returnArr[i] = (int *)malloc(sero * sizeof(int));
	}
	//�޾ƿ¹迭�� ������ ���θ������ �迭�� 90�� ȸ���� ���·� �־��ش�.
	for (i = 0; i < garo; ++i) {
		for (j = 0; j < sero; ++j) {
			returnArr[i][j] = arr[sero -1- j][i];
		}
	}
	//������ ���� �޸� ����
	for (i = 0; i < sero; ++i) {
		free(arr[i]);
	}

	//������ �޸� ����
	free(arr);

	//ȸ����Ų �迭�� �������ش�
	return returnArr;
}

int main() {
	int garo, sero,i,j;
	int** arr = NULL;

	//����ڿ��� ���ο� ����ũ�⸦ �Է¹޴´�
	printf("input row size of the matrix : ");
	scanf("%d", &sero);
	printf("input column size of the matrix : ");
	scanf("%d", &garo);

	//������ ���� �Ҵ�
	arr = (int **)malloc(sero*sizeof(int *));

	//������ �������� �Ҵ�
	for (i = 0; i < sero; ++i) {
		arr[i] = (int *)malloc(garo * sizeof(int));

		//���� �־��ش�
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
	//0�� �迭����Ʈ
	printf("< degree : 0 >\n");
	print(arr, sero, garo);
	
	//90�� ȸ���迭����Ʈ
	printf("< degree : 90 >\n");
	arr = rotate(arr);
	print(arr, garo, sero);
	
	//180�� ȸ�� �迭 ����Ʈ
	printf("< degree : 180 >\n");
	arr = rotate(arr);
	print(arr, sero, garo);
	
	//270�� ȸ�� �迭 ����Ʈ
	printf("< degree : 270 >\n");
	arr = rotate(arr);
	print(arr, garo, sero);

	
	
	







}