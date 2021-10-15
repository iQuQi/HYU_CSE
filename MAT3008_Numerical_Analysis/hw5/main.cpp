#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <complex>
#include <iostream>
using namespace std;
void lubksb(float **a, int* indx, float * b, int n);
void mprove(float **a, float **alud, int* indx, float * b, float * x, int a_n, int x_n);
void SWAP(float *a, float *b);
float MAX(float a, float b);
float MIN(float a, float b);
float SIGN(const float &a, const double &b);
void svdcmp(float**a, float*w, float**v, int n, int m);
void gaussj(float ** a, float **b, int n, int m);
void ludcmp(float **a, int *indx, float &d, int n);
void load(FILE *fp);
float SQR(float a);
float pythag(const float a, const float b);
float** transMatrix(float **mat1, int n);
float** multiMatrix(float **mat1, float **mat2, int row_1, int col_1,int col_2);
float DetMat(float **mat, int size);
float CofacMat(float **mat, int p, int q, int size);
void mprove(float** a, float** alud, int* indx, float* b, float* x, int n);


float **A = NULL;
float *b = NULL;
int N;

float *problem_gaussj(int N, float **A, float *b) {
	//동적할당을 통해 A와 B를 복사해둔다
	float **A_copy = (float**)calloc(N , sizeof(float *));
	float **B_copy = (float **)calloc(N , sizeof(float *));

	for (int i = 0; i < N; i++) {
		A_copy[i] = (float*)malloc((N) * sizeof(float));
		memcpy(A_copy[i], A[i], (N) * sizeof(float));
	}
	
	for (int i = 0; i < N; i++) {
		B_copy[i] = (float*)calloc(1, sizeof(float));
		B_copy[i][0] = b[i];
	}

	//가우스 조던 소거법 사용
	gaussj(A_copy, B_copy, N, 1);
	//찾은 해를 옮긴다.
	float *ret = (float *)calloc(N , sizeof(float));
	for (int i = 0; i < N; i++) {
		ret[i] = B_copy[i][0];
	}
	
	//동적할당한 공간 해제하기
	for (int i = 0; i < N; i++) free(A_copy[i]);
	for (int i = 0; i < N; i++) free(B_copy[i]);
	free(A_copy);
	free(B_copy);

	return ret;
}

float *problem_ludcmp(int N, float **A, float *b) {
	float d = 0;
	int * indx = (int *)malloc(sizeof(int)*N);

	//동적할당을 통해 A와 B를 복사해둔다
	float **A_copy = (float**)malloc((N) *sizeof(float *));
	float *B_copy = (float *)malloc((N) *sizeof(float ));

	for (int i = 0; i < N; i++) {
		A_copy[i] = (float*)malloc((N) *sizeof(float));
		memcpy(A_copy[i], A[i], (N) * sizeof(float));
	}

	B_copy = (float*)malloc((N) *sizeof(float));
	for (int i = 0; i < N; i++) {
		B_copy[i] = b[i];
	}

	//LU Decomposition 사용
	ludcmp(A_copy, indx, d, N);

	lubksb(A_copy, indx, B_copy, N);

	//찾은 해를 옮긴다.
	float *ret = (float *)calloc(N , sizeof(float));
	for (int i = 0; i < N; i++) ret[i] = B_copy[i];


	//동적할당한 공간 해제하기
	for (int i = 0; i < N; i++) free(A_copy[i]);
	free(A_copy);
	free(B_copy);

	return ret;
}

float *problem_svdcmp(int N, float **A, float *b) {
	float * w = (float*)calloc(N,sizeof(float));
	float **v= (float**)calloc(N,sizeof(float*));
	for (int i = 0; i < N; i++)
		v[i]= (float*)calloc(N,sizeof(float));
	
	float **x;

	//동적할당을 통해 A와 B를 복사해둔다
	float **A_copy = (float**)calloc(N, sizeof(float *));
	float **w_ = (float**)calloc(N, sizeof(float *));
	float **B_copy = (float **)calloc(N, sizeof(float *));

	for (int i = 0; i < N; i++) {
		w_[i] = (float*)calloc(N, sizeof(float));
		A_copy[i] = (float*)calloc(N, sizeof(float));
		memcpy(A_copy[i], A[i], (N) * sizeof(float));
	}

	for (int i = 0; i < N; i++) {
		B_copy[i] = (float*)calloc(1, sizeof(float));
		B_copy[i][0] = b[i];
	}

	//homo 계산인지 아닌지 판별
	int homo = 1;
	for (int i = 0; i < N; i++)
	{
		if (b[i] != 0) homo = 0;
	}

	//SVD를 실행한다
	svdcmp(A_copy, w, v, N, 1);

	//b=0이라면
	if (homo) {
		//Singular Value Decomposition (SVD) 사용
		for (int i = 0; i < N; i++) {
			if (w[i] == 0) {
				for (int j = 0; j < N; j++)
				{
					x[i][0] = v[i][j];
				}
			}
		}
	}
	//b!=0이라면
	else {
		float ** tmp1, **tmp2;
		for (int i = 0; i < N; i++)
		{
			w_[i][i] = ((w[i]==0) ? 0 : float(1/w[i]));

		}
		tmp1 = multiMatrix(transMatrix(A_copy, N), B_copy, N, N, 1);
		tmp2 = multiMatrix(v, w_, N, N, N);
		x=multiMatrix(tmp2, tmp1 , N,N,1);
	}

	//찾은 해를 옮긴다.
	float *ret = (float *)calloc(N , sizeof(float));
	for (int i = 0; i < N; i++) ret[i] = x[i][0];

	//동적할당한 공간 해제하기
	for (int i = 0; i < N; i++) free(A_copy[i]);
	free(A_copy);
	free(B_copy);

	return ret;
}

float ** problem_inverse_matrix(int N, float **A) {
	float* b = (float *)malloc(sizeof(float)*N);
	float **A_inverse = (float**)calloc(N , sizeof(float *));
	float d;
	int * indx = (int *)malloc(sizeof(int)*N);

	//동적할당을 통해 A와 B를 복사해둔다
	float **A_copy = (float**)malloc(N * sizeof(float *));
	for (int i = 0; i < N; i++) {
		A_copy[i] = (float*)malloc(N * sizeof(float));
		A_inverse[i] = (float*)malloc(N * sizeof(float));
		memcpy(A_copy[i], A[i], N  * sizeof(float));
	}
	//LU Decomposition 사용
	ludcmp(A_copy, indx, d, N);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (i == j) b[j] = 1;
			else b[j] = 0;
		}

		lubksb(A_copy, indx, b, N);
		for (int j = 0; j < N; j++)
		{
			A_inverse[j][i] = b[j];
		}
	}
	//동적할당한 공간 해제하기
	for (int i = 0; i < N; i++) free(A_copy[i]);
	free(A_copy);

	return A_inverse;
}


float* problem_improve(float*b,float*x) {
	float d;
	int * indx = (int *)malloc(sizeof(int)*N);

	//동적할당을 통해 A와 x를 복사해둔다
	float **A_copy = (float**)malloc(N * sizeof(float *));
	float *x_copy = (float*)malloc(N * sizeof(float));

	for (int i = 0; i < N; i++) {
		A_copy[i] = (float*)malloc(N * sizeof(float));
		memcpy(A_copy[i], A[i], N * sizeof(float));
		x_copy[i] = x[i];
	}

	//LU Decomposition 사용
	ludcmp(A_copy, indx, d, N);
	mprove(A, A_copy, indx, b, x_copy, N);

	return x_copy;
}

int main() {
	FILE *fp;
	float *x_1, *x_2, *x_3, *x_1_pr , *x_2_pr , *x_3_pr;
	char fileName[30];
	scanf("%s", fileName);

	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("Failed to open file: %s\n",fileName);
	}
	load(fp);

	x_1=problem_gaussj(N, A, b);

	x_2 = problem_ludcmp(N, A, b);
	x_2_pr = problem_improve(b,x_2);

	x_3=problem_svdcmp(N, A, b);

	//3가지 방법으로 해 구하기
	printf("1-1) Gauss-Jordan Eliminating : x = { ");
	for (int i = 0; i < N; i++)
		printf("%f ", x_1[i]);
	printf("}\n\n");

	printf("2-1) LU Decompositon : x = { ");
	for (int i = 0; i < N; i++)
		printf("%f ", x_2[i]);
	printf("}\n");
	printf("2-2) Iterative Improve : x = { ");
	for (int i = 0; i < N; i++)
		printf("%f ", x_2_pr[i]);
	printf("}\n\n");

	printf("3-1) Singular Value Decomposition : x = { ");
	for (int i = 0; i < N; i++)
		printf("%f ", x_3[i]);
	printf("}\n");


	//A의 inverse Matrix 구하기
	printf("Inverse A Matrix : \n");
	float ** A_inverse=problem_inverse_matrix(N,A);
	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < N; i++)
		{
			printf("%f ", A_inverse[j][i]);
		}
		printf("\n");
	}

	printf("\nDet(A) = %f", DetMat(A,N));

	for (int i = 0; i < N; i++)
		free(A[i]);
	free(A);
	free(b);
	fclose(fp);

	return 0;
}

void lubksb(float **a, int* indx, float * b,int n)
{
	int i, ii = 0, ip, j;
	float sum;

	for (i = 0; i < n; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii != 0)
			for (j = ii - 1; j < i; j++) sum -= a[i][j] * b[j];
		else if (sum != 0.0)
			ii = i + 1;
		b[i] = sum;
	}
	for (i = n - 1; i >= 0; i--) {
		sum = b[i];
		for (j = i + 1; j < n; j++) sum -= a[i][j] * b[j];
		b[i] = sum / a[i][i];
	}
}

void mprove(float **a, float **alud, int* indx, float * b, float * x, int a_n, int x_n)
{
	int i, j;

	float * r;
	r = (float*)malloc(sizeof(float)*x_n);
	
	for (i = 0; i < x_n; i++) {
		long double sdp = -b[i];
		for (j = 0; j < x_n; j++)
			sdp += (long double)a[i][j] * (long double)x[j];
		r[i] = sdp;
	}
	lubksb(alud, indx, r, a_n);
	for (i = 0; i < x_n; i++) x[i] -= r[i];
}


void SWAP(float *a, float *b)
{
	float dum = *a; 
	*a = *b; 
	*b = dum;
}

float MAX(float a, float b)
{
	return a > b ? a : b;
}

float MIN(float a, float b)
{
	return a < b ? a : b;

}

float SIGN(const float &a, const double &b)
{
	return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
}


void svdcmp(float**a, float*w, float**v, int n, int m)
{
	bool flag;
	int i, its, j, jj, k, l, nm;
	float anorm, c, f, g, h, s, scale, x, y, z;

	float* rv1;
	rv1 = (float*)malloc(sizeof(float)*n);

	g = scale = anorm = 0.0;
	for (i = 0; i < n; i++) {
		l = i + 2;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++) scale += fabs(a[k][i]);
			if (scale != 0.0) {
				for (k = i; k < m; k++) {
					a[k][i] /= scale;
					s += a[k][i] * a[k][i];
				}
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = f - g;
				for (j = l - 1; j < n; j++) {
					for (s = 0.0, k = i; k < m; k++) s += a[k][i] * a[k][j];
					f = s / h;
					for (k = i; k < m; k++) a[k][j] += f * a[k][i];
				}
				for (k = i; k < m; k++) a[k][i] *= scale;
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i + 1 <= m && i != n) {
			for (k = l - 1; k < n; k++) scale += fabs(a[i][k]);
			if (scale != 0.0) {
				for (k = l - 1; k < n; k++) {
					a[i][k] /= scale;
					s += a[i][k] * a[i][k];
				}
				f = a[i][l - 1];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l - 1] = f - g;
				for (k = l - 1; k < n; k++) rv1[k] = a[i][k] / h;
				for (j = l - 1; j < m; j++) {
					for (s = 0.0, k = l - 1; k < n; k++) s += a[j][k] * a[i][k];
					for (k = l - 1; k < n; k++) a[j][k] += s * rv1[k];
				}
				for (k = l - 1; k < n; k++) a[i][k] *= scale;
			}
		}
		anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (g != 0.0) {
				for (j = l; j < n; j++)
					v[j][i] = (a[i][j] / a[i][l]) / g;
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++) s += a[i][k] * v[k][j];
					for (k = l; k < n; k++) v[k][j] += s * v[k][i];
				}
			}
			for (j = l; j < n; j++) v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = MIN(m, n) - 1; i >= 0; i--) {
		l = i + 1;
		g = w[i];
		for (j = l; j < n; j++) a[i][j] = 0.0;
		if (g != 0.0) {
			g = 1.0 / g;
			for (j = l; j < n; j++) {
				for (s = 0.0, k = l; k < m; k++) s += a[k][i] * a[k][j];
				f = (s / a[i][i])*g;
				for (k = i; k < m; k++) a[k][j] += f * a[k][i];
			}
			for (j = i; j < m; j++) a[j][i] *= g;
		}
		else for (j = i; j < m; j++) a[j][i] = 0.0;
		++a[i][i];
	}
	for (k = n - 1; k >= 0; k--) {
		for (its = 0; its < 30; its++) {
			flag = true;
			for (l = k; l >= 0; l--) {
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = false;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm) break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l - 1; i < k + 1; i++) {
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if (fabs(f) + anorm == anorm) break;
					g = w[i];
					h = pythag(f, g);
					w[i] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 0; j < m; j++) {
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y * c + z * s;
						a[j][i] = z * c - y * s;
					}
				}
			}
			z = w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j = 0; j < n; j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 29) printf("no convergence in 30 svdcmp iterations");
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z)*(y + z) + (g - h)*(g + h)) / (2.0*h*y);
			g = pythag(f, 1.0);
			f = ((x - z)*(x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 0; jj < n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x * c + z * s;
					v[jj][i] = z * c - x * s;
				}
				z = pythag(f, h);
				w[j] = z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for (jj = 0; jj < m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y * c + z * s;
					a[jj][i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
}


void gaussj(float ** a, float **b, int n, int m)
{
	int i, icol, irow, j, k, l, ll;
	float big, dum, pivinv;

	int* indxc, *indxr, *ipiv;
	indxc = (int *)malloc(sizeof(int)*n);
	indxr = (int *)malloc(sizeof(int)*n);
	ipiv = (int *)malloc(sizeof(int)*n);

	for (j = 0; j < n; j++) ipiv[j] = 0;
	for (i = 0; i < n; i++) {
		big = 0.0;
		for (j = 0; j < n; j++)
			if (ipiv[j] != 1)
				for (k = 0; k < n; k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					}
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l = 0; l < n; l++) SWAP(&a[irow][l], &a[icol][l]);
			for (l = 0; l < m; l++) SWAP(&b[irow][l], &b[icol][l]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (a[icol][icol] == 0.0) printf("gaussj: Singular Matrix\n");
		pivinv = 1.0 / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 0; l < n; l++) a[icol][l] *= pivinv;
		for (l = 0; l < m; l++) b[icol][l] *= pivinv;
		for (ll = 0; ll < n; ll++)
			if (ll != icol) {
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 0; l < n; l++) a[ll][l] -= a[icol][l] * dum;
				for (l = 0; l < m; l++) b[ll][l] -= b[icol][l] * dum;
			}
	}
	for (l = n - 1; l >= 0; l--) {
		if (indxr[l] != indxc[l])
			for (k = 0; k < n; k++)
				SWAP(&a[k][indxr[l]], &a[k][indxc[l]]);
	}
}



void ludcmp(float **a, int *indx, float &d, int n)
{
	const float TINY = 1.0e-20;
	int i, imax, j, k;
	float big, dum, sum, temp;

	float* vv;
	vv = (float*)malloc(sizeof(float)*n);

	d = 1.0;
	for (i = 0; i < n; i++) {
		big = 0.0;
		for (j = 0; j < n; j++)
			if ((temp = fabs(a[i][j])) > big) big = temp;
		if (big == 0.0) printf("Singular matrix in routine ludcmp\n");
		vv[i] = 1.0 / big;
	}
	for (j = 0; j < n; j++) {
		for (i = 0; i < j; i++) {
			sum = a[i][j];
			for (k = 0; k < i; k++) sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
		}
		big = 0.0;
		for (i = j; i < n; i++) {
			sum = a[i][j];
			for (k = 0; k < j; k++) sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k = 0; k < n; k++) {
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			d = -d;
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a[j][j] == 0.0) a[j][j] = TINY;
		if (j != n - 1) {
			dum = 1.0 / (a[j][j]);
			for (i = j + 1; i < n; i++) a[i][j] *= dum;
		}
	}
}


void load(FILE *fp) {
	int M;
	//A의 크기를 읽어온다
	fscanf(fp, "%d %d", &M, &N);

	//A와 b의 값을 읽어온다
	A = (float**)malloc(N*sizeof(float *));
	for (int i = 0; i < N; i++)
		A[i] = (float*)malloc(N*sizeof(float));

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			fscanf(fp, "%f", &A[i][j]);

	b = (float*)malloc(N*sizeof(float));
	for (int i = 0; i < N; i++)
		fscanf(fp, "%f", &b[i]);


}


float SQR(float a) { 
	return a * a; 
}

float pythag(const float a, const float b)
{
	float absa, absb;

	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) return absa * sqrt(1.0 + SQR(absb / absa));
	else return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
}

// transformation 매트릭스를 산출해 내는 함수
float** transMatrix(float **mat1, int n) {
	int i, j;

	float** ret = (float**)malloc(sizeof(float*)*n);
	for (int i = 0; i < n; i++)
	{
		ret[i] = (float*)malloc(sizeof(float)*n);
	}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			ret[j][i] = mat1[i][j];


	return ret;
}

// 두 매트릭스의 곱을 구하는 함수
float** multiMatrix(float **mat1, float **mat2, int row_1 ,int col_1, int col_2 ) {
	int i, j, k;
	float resTemp = 0.0;


	float** ret = (float**)malloc(sizeof(float*)*row_1);
	for (int i = 0; i < row_1; i++)
	{
		ret[i] = (float*)malloc(sizeof(float)*col_2);
	}

	for (i = 0; i < row_1; i++) {
		for (j = 0; j < col_2; j++) {
			for (k = 0; k < col_1; k++) {
				resTemp += mat1[i][k] * mat2[k][j];
			}
			ret[i][j] = resTemp;
			resTemp = 0.0;
		}
	}


	return ret;
}

float DetMat(float **mat, int size) {
	int p = 0, q = 0;
	float det = 0;

	if (size == 1) {   //determinant of 1 by 1 matrix
		return mat[0][0];
	}
	else if (size == 2) { //determinant 2 by 2 matrix
		return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	}
	else {     //3 by 3 이상
		for (q = 0, det = 0; q < size; q++) {
			det = det + mat[0][q] * CofacMat(mat, 0, q, size);
		}
		return det;
	}
	return 0;
}

float CofacMat(float **mat, int p, int q, int size) {
	int i = 0, j = 0;  //인자로 받은 matrix의 index
	int x = 0, y = 0;  //cmat의 index
	float **cmat;
	float cofactor = 0;

	//cofactor matrix의 dynamic memory 할당
	cmat = (float**)malloc(sizeof(float*) * (size - 1));
	for (i = 0; i < (size - 1); i++) {
		cmat[i] = (float*)malloc(sizeof(float) * (size - 1));
	}

	//mat으로 부터 cmat추출(cmat은 mat의 p행과 q열의 원소를 제외한 나머지 원소들로 구성된 matrix)
	for (i = 0, x = 0; i < size; i++) {
		if (i != p) {
			for (j = 0, y = 0; j < size; j++) {
				if (j != q) {
					cmat[x][y] = mat[i][j];
					y++;
				}
			}
			x++;
		}
	}

	//cofactor를 계산
	cofactor = pow(-1, p)*pow(-1, q)*DetMat(cmat, size - 1);

	//cofactor matrix의 dynamic memory 해제
	for (i = 0; i < (size - 1); i++) {
		free(cmat[i]);
	}
	free(cmat);

	return cofactor;
}


void mprove(float** a, float** alud, int* indx, float* b,float* x,int n)
{
	int i, j;

	//int n = x.size();
	float* r;
	r = (float*)calloc(n, sizeof(float));
	for (i = 0; i < n; i++) {
		long double sdp = -b[i];
		for (j = 0; j < n; j++)
			sdp += (long double)a[i][j] * (long double)x[j];
		r[i] = sdp;
	}
	lubksb(alud, indx, r ,n);
	for (i = 0; i < n; i++) x[i] -= r[i];
}
