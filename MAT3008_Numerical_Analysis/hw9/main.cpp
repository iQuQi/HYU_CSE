#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <cmath>
using namespace std;
void svdcmp(float**a, float*w, float**v, int n, int m);
float** multiMatrix(float **mat1, float **mat2, int row_1, int col_1, int col_2);
void SWAP(float *a, float *b);
float MAX(float a, float b);
float MIN(float a, float b);
float SIGN(const float &a, const double &b);
float pythag(const float a, const float b);
float SQR(float a);
float** transMatrix(float **mat1, int n);
float** make_matrix(int n, int m);
float **problem_svdcmp(float **A, float **b, int M, int K);
void gaussj(float ** a, float **b, int n, int m);
float **fit_data(float **f, float **y, int n, int m, int k);

typedef struct {
	float x;
	float y;
	float xp;
	float yp;
} Data;

typedef struct {
	Data *data;
	int size;
	int _capacity;
} DataList;


int main() {
	//파일 받아오기
	char filename[30];
	scanf("%s", filename);
	FILE *fp = fopen(filename, "r");

	//데이터 리스트 초기화
	DataList *list = (DataList*)malloc(sizeof(DataList));
	list->data = (Data*)malloc(sizeof(Data));
	list->size = 0;
	list->_capacity = 1;

	//데이터 입력받기
	Data *data = &list->data[0];

	while (fscanf(fp, "%f %f %f %f", &data->x, &data->y, &data->xp, &data->yp) == 4) {
		list->size++;
		//용량 초과시 용량 늘려주기
		if (list->size == list->_capacity) {
			list->_capacity *= 2;
			list->data = (Data*)realloc(list->data, sizeof(Data) * (list->_capacity));
		}
		data = &list->data[list->size];
	}
	fclose(fp);

	// f=> n * m matrix
	// y=> n * k matrix
	// a=> m * k matrix
	int n = list->size;
	int m = 3;
	int k = 2;

	//매트릭스 생성
	float **f = make_matrix(n, m);
	float **y = make_matrix(n, k);

	for (int i = 1; i <= n; i++) {
		f[i][1] = list->data[i - 1].x;
		f[i][2] = list->data[i - 1].y;
		f[i][3] = 1;
		//ax+ay+a => 관찰 데이터

		y[i][1] = list->data[i - 1].xp;//x'
		y[i][2] = list->data[i - 1].yp;//y'
	}
	//printf("1\n");
	//매개변수
	float **a = fit_data(f, y, n, m, k);
	int cnt = 1;
	for (int i = 1; i <= m; i++)
	{
		for (int j= 1; j <= k; j++)
		{
			printf("[a[%d]]%f ",i+(j-1)*3 ,a[i][j]);
		}
		printf("\n");
	}


}

//fit 함수
float **fit_data(float **f, float **y, int n, int m, int k) {
	//새 매트릭스 생성
	float **fit_f = make_matrix(m, m);
	float **fit_y = make_matrix(m, k);
	//printf("2-fit\n");

	for (int i = 1; i <= m; i++) {
		for (int j = 1; j <= m; j++) {
			for (int k = 1; k <= n; k++)
				fit_f[i][j] += f[k][i] * f[k][j];//F_t*F
		}
	}
	//printf("3-fit\n");

	for (int i = 1; i <= m; i++) {
		for (int j = 1; j <= k; j++) {
			for (int k = 1; k <= n; k++)
				fit_y[i][j] += f[k][i] * y[k][j]; //F_t*y
		}
	}
	//F_t*F*a=F_t*y 
	//해(a) 구하기
	return problem_svdcmp(fit_f, fit_y, m, k);


}

//매트릭스 생성함수
float** make_matrix(int n, int m) {
	float **new_mat = (float**)calloc((n + 1), sizeof(float*));
	for (int i = 0; i <= n; i++)
		new_mat[i] = (float*)calloc((m + 1), sizeof(float));

	return new_mat;
}

//============================================================================================

float pythag(const float a, const float b)
{
	float absa, absb;

	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) return absa * sqrt(1.0 + SQR(absb / absa));
	else return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
}

float SQR(float a) {
	return a * a;
}


void SWAP(float * a,float* b) {
	float tmp = *a;
	*a = *b;
	*b = tmp;
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




// 두 매트릭스의 곱을 구하는 함수
float** multiMatrix(float **mat1, float **mat2, int row_1, int col_1, int col_2) {
	int i, j, k;
	float resTemp = 0.0;


	float** ret = (float**)malloc(sizeof(float*)*row_1);
	for (int i = 1; i < row_1; i++)
	{
		ret[i] = (float*)malloc(sizeof(float)*col_2);
	}

	for (i = 1; i < row_1; i++) {
		for (j = 1; j < col_2; j++) {
			for (k = 1; k < col_1; k++) {
				resTemp += mat1[i][k] * mat2[k][j];
			}
			ret[i][j] = resTemp;
			resTemp = 0.0;
		}
	}


	return ret;
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


// transformation 매트릭스를 산출해 내는 함수
float** transMatrix(float **mat1, int n) {
	int i, j;

	float** ret = (float**)malloc(sizeof(float*)*n);
	for (int i = 1; i < n; i++)
	{
		ret[i] = (float*)malloc(sizeof(float)*n);
	}

	for (i = 1; i < n; i++)
		for (j = 1; j < n; j++)
			ret[j][i] = mat1[i][j];


	return ret;
}



float **problem_svdcmp(float **A, float **b,int M,int K) {
	float * w = (float*)calloc(M+1, sizeof(float));


	float **w_ = (float**)calloc(M+1, sizeof(float *));
	float **x  = (float**)calloc(M+1, sizeof(float *));
	float **v = (float**)calloc(M+1, sizeof(float*));

	for (int i = 0; i < M+1; i++) {
		w_[i] = (float*)calloc(M+1, sizeof(float));
		x[i] = (float*)calloc(K+1, sizeof(float));
		v[i] = (float*)calloc(M+1, sizeof(float));

	}

	//printf("4-svd\n");



		//SVD를 실행한다
		svdcmp(A, w, v, M+1, K+1);
		//A를 U W V로 분리하고 U는 A에 담아서, 나머지도 각각 w v에 담아서 돌려줌
		//printf("5-svd end\n");

		for (int k = 1; k <= K; k++)
		{
		//	printf("6-svd: %d\n", k);

			//homo 계산인지 아닌지 판별
			int homo = 1;
			for (int i = 1; i <= M; i++)
			{
				if (b[i][k] != 0) homo = 0;
			}

			//b=0이라면
			if (homo) {
				//Singular Value Decomosition (SVD) 사용
				for (int i = 1; i <= M; i++) {
					if (w[i] == 0) {
						for (int j = 1; j <= M; j++)
						{
							x[i][k] = v[i][j];
						}
					}
				}
			//	printf("7-homo\n");

			}
			//b!=0이라면
			else {
				//printf("8-not homo\n");

				float ** tmp1, **tmp2;
				for (int i = 1; i <= M; i++)
				{
					w_[i][i] = ((w[i] == 0) ? 0 : float(1 / w[i]));

				}
				tmp1 = multiMatrix(transMatrix(A, M+1), b, M+1, M+1, K+1);
				tmp2 = multiMatrix(v, w_, M+1, M+1, M+1);
				//printf("9-mult mat1\n");

				x = multiMatrix(tmp2, tmp1, M+1, M+1, K+1);
				//printf("10-mult mat2\n");

			}

		}

	
	return x;
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
		if (a[icol][icol] == 0.0) printf("gaussj: Singular Matrix");

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


