// #define _SHOW_CORRESPONDENCE_IMG

#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
using namespace cv;
using namespace std;

void SWAP(float*a, float*b) {
	float tmp = *a;
	*a = *b;
	*b = tmp;
}


void gaussj(float**a, float**b, int n, int m)
{
	int i, icol, irow, j, k, l, ll;
	float big, dum, pivinv;
	//printf("gauss1\n");
	int * indxc, *indxr, *ipiv;
	indxc = (int*)calloc(n, sizeof(int));
	indxr = (int*)calloc(n, sizeof(int));
	ipiv = (int*)calloc(n, sizeof(int));

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
					//printf("gauss2\n");
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l = 0; l < n; l++) SWAP(&a[irow][l], &a[icol][l]);
			for (l = 0; l < m; l++) SWAP(&b[irow][l], &b[icol][l]);
		}	//printf("gauss3\n");

		indxr[i] = irow;
		indxc[i] = icol;
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
			}	//printf("gauss4\n");

	}	//printf("gauss5\n");

	for (l = n - 1; l >= 0; l--) {
		if (indxr[l] != indxc[l])
			for (k = 0; k < n; k++)
				SWAP(&a[k][indxr[l]], &a[k][indxc[l]]);
	}	//printf("gauss6\n");

}


void mrqcof(float *x, float* y, float* sig, float* a,
	int *ia, float**alpha, float* beta, float *chisq,
	void funcs(const float, float* , float *, float*),int ma, int ndata)
{
	int i, j, k, l, m, mfit = 0;
	float ymod;
	float wt, sig2i, dy;

	//int ndata = x.size();
	//int ma = a.size();
	float* dyda = (float*)calloc(ma, sizeof(float));
	for (j = 0; j < ma; j++)
		if (ia[j]) mfit++;
	for (j = 0; j < mfit; j++) {
		for (k = 0; k <= j; k++) alpha[j][k] = 0.0;
		beta[j] = 0.0;
	}
	*chisq = 0.0;
	for (i = 0; i < ndata; i++) {
		funcs(x[i], a, &ymod, dyda);
		sig2i = 1.0 / (sig[i] * sig[i]);
		dy = y[i] - ymod;
		for (j = 0, l = 0; l < ma; l++) {
			if (ia[l]) {
				wt = dyda[l] * sig2i;
				for (k = 0, m = 0; m < l + 1; m++)
					if (ia[m]) alpha[j][k++] += wt * dyda[m];
				beta[j++] += dy * wt;
			}
		}
		*chisq += dy * dy*sig2i;
	}
	for (j = 1; j < mfit; j++)
		for (k = 0; k < j; k++) alpha[k][j] = alpha[j][k];
}


void covsrt(float ** covar, int ia[], const int mfit,int ma)
{
	int i, j, k;

	for (i = mfit; i < ma; i++)
		for (j = 0; j < i + 1; j++) covar[i][j] = covar[j][i] = 0.0;
	k = mfit - 1;
	for (j = ma - 1; j >= 0; j--) {
		if (ia[j]) {
			for (i = 0; i < ma; i++) SWAP(&covar[i][k], &covar[i][j]);
			for (i = 0; i < ma; i++) SWAP(&covar[k][i], &covar[j][i]);
			k--;
		}
	}
}


void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **covar, float **alpha, float *chisq,
	void(*funcs)(float, float[], float *, float[]), float *alamda){
	static int mfit;
	static float ochisq;
	int j, k, l;
	//printf("mrq1\n");
	static float** oneda = (float**)calloc(ma, sizeof(float*));
	for (int i = 0; i < ma; i++)
	{
		//printf("mrq1..??^^.\n");

		oneda[i] = (float*)calloc(1, sizeof(float));
	}
	
	static float* atry = (float*)calloc(ma, sizeof(float));
	static float *beta = (float*)calloc(ma, sizeof(float));
	static float* da = (float*)calloc(ma, sizeof(float));
	//printf("mrq2\n");

	if (*alamda < 0.0) {
		mfit = 0;
		for (j = 0; j < ma; j++)
			if (ia[j]) mfit++;
		*alamda = 0.001;
		mrqcof(x, y, sig, a, ia, alpha, beta, chisq, funcs,ma,ndata);
		ochisq = *chisq;
		for (j = 0; j < ma; j++) atry[j] = a[j];
	}

	static float** temp = (float**)calloc(mfit, sizeof(float*));
	for (int i = 0; i < ma; i++)
	{
		temp[i] = (float*)calloc(mfit, sizeof(float));
	}
	//printf("mrq3\n");

	for (j = 0; j < mfit; j++) {
		for (k = 0; k < mfit; k++) covar[j][k] = alpha[j][k];
		//printf("mrq4-1\n");

		covar[j][j] = alpha[j][j] * (1.0 + *alamda);
		

		for (k = 0; k < mfit; k++) {
			//printf("mrq4-2 %f %f", temp[j][k], covar[j][k]);
			temp[j][k] = covar[j][k];
			//printf("mrq4-2....\n");

		}
		//printf("mrq4-3 %f \n",beta[j]);

		oneda[j][0] = beta[j];
		//printf("mrq4-4\n");

	}

	gaussj(temp, oneda,mfit,ma);
	//printf("mrq4-2\n");

	for (j = 0; j < mfit; j++) {
		for (k = 0; k < mfit; k++) covar[j][k] = temp[j][k];
		da[j] = oneda[j][0];
	}
	if (*alamda == 0.0) {
		covsrt(covar, ia, mfit,ma);
		covsrt(alpha, ia, mfit,ma);
		return;
	}
	for (j = 0, l = 0; l < ma; l++)
		if (ia[l]) atry[l] = a[l] + da[j++];
	mrqcof(x, y, sig, atry, ia, covar, da, chisq, funcs,ma,ndata);
	if (*chisq < ochisq) {
		*alamda *= 0.1;
		ochisq = *chisq;
		for (j = 0; j < mfit; j++) {
			for (k = 0; k < mfit; k++) alpha[j][k] = covar[j][k];
			beta[j] = da[j];
		}
		for (l = 0; l < ma; l++) a[l] = atry[l];
	}
	else {
		*alamda *= 10.0;
		*chisq = ochisq;
	}
}



struct Data {
	float x;
	float y;
	float xp;
	float yp;
};

static vector<Data> data_vec;





void func_xp(float x, float a[], float *y, float dyda[]) {
	auto &d = data_vec[roundf(x)];
	float divisor = (a[7] * d.x + a[8] * d.y + 1);
	*y = (a[1] * d.x + a[2] * d.y + a[3]) / divisor;
	dyda[1] = d.x / divisor;
	dyda[2] = d.y / divisor;
	dyda[3] = 1 / divisor;
	dyda[7] = -d.x * *y / divisor;
	dyda[8] = -d.y * *y / divisor;
}


void func_yp(float x, float a[], float *y, float dyda[]) {
	auto &d = data_vec[roundf(x)];
	float divisor = (a[7] * d.x + a[8] * d.y + 1);
	*y = (a[4] * d.x + a[5] * d.y + a[6]) / divisor;
	dyda[4] = d.x / divisor;
	dyda[5] = d.y / divisor;
	dyda[6] = 1 / divisor;
	dyda[7] = -d.x * *y / divisor;
	dyda[8] = -d.y * *y / divisor;
}


void run_mrqmin(vector<pair<Point2f, Point2f>> &correspondences, float a[], const int ma) {
	int ndata = correspondences.size();
	float *x = (float*)calloc(ndata, sizeof(float));
	float *y = (float*)calloc(ndata, sizeof(float));
	float *sig = (float*)calloc(ndata, sizeof(float));
	int *ia = (int*)calloc(ma, sizeof(int));
	//printf("run1\n");
	float **covar = (float**)calloc(ma, sizeof(float*));
	float **alpha = (float**)calloc(ma, sizeof(float*));
	for (int i = 0; i < ma; i++)
	{
		covar[i] = (float*)calloc(ma, sizeof(float));
		alpha[i] = (float*)calloc(ma, sizeof(float));
	}
	float chisq;
	float alamda;
	//printf("run2\n");

	iota(x + 1, x + ndata + 1, 1);
	fill(sig + 1, sig + ndata + 1, 1);
	for_each(correspondences.begin(), correspondences.end(), [](const auto &i) {
		data_vec.push_back({ i.first.x, i.first.y, i.second.x, i.second.y });
	});

	for (int i = 1; i <= ndata; i++) y[i] = data_vec[i - 1].xp;
	ia[1] = ia[2] = ia[3] = ia[7] = ia[8] = 1;
	alamda = -1;
	mrqmin(x, y, sig, ndata, a, ia, ma, covar, alpha, &chisq, func_xp, &alamda);
	//printf("run3\n");

	for (int i = 1; i <= ndata; i++) y[i] = data_vec[i - 1].yp;
	ia[1] = ia[2] = ia[3] = 0;
	ia[4] = ia[5] = ia[6] = 1;
	alamda = -1;
	mrqmin(x, y, sig, ndata, a, ia, ma, covar, alpha, &chisq, func_yp, &alamda);
	//printf("run4\n");


}


int main() {
	const char * fileName="sample_data.txt";
	FILE* fp = fopen(fileName, "r");
	//구현을 완료하지 못했습니다. 죄송합니다

	const int ma = 8;
	float *a = (float*)calloc(ma, sizeof(float));
	fill(a + 1, a + ma + 1, 2); 
	vector<pair<Point2f, Point2f>> correspondences;
	float p1_x, p1_y, p2_x, p2_y;
	while (fscanf(fp,"%f %f %f %f",&p1_x,&p1_y,&p2_x,&p2_y)>0) {
		Point2f a(p1_x, p1_y);
		Point2f b(p2_x, p2_y);
		correspondences.push_back({a ,b});
		//printf("scanf ");
	}

	//cout << "?..";
	run_mrqmin(correspondences, a, ma);
	cout << "a = [";
	for (int i = 1; i < ma; i++) {
		cout << a[i] << ", ";
	}
	cout << a[ma] << "]" << endl;

	return 0;
}


