#include <stdio.h>
#include <time.h>
#include <cmath>
using namespace std;

void rot(float** a, const float s, const float tau, const int i,
	const int j, const int k, const int l)
{
	float g, h;

	g = a[i][j];
	h = a[k][l];
	a[i][j] = g - s * (h + g * tau);
	a[k][l] = h + s * (g - h * tau);
}

void jacobi(float**a, float*d, float**v, int &nrot,int d_size)
{
	int i, j, ip, iq;
	float tresh, theta, tau, t, sm, s, h, g, c;

	int n = d_size;
	float* b = (float *)malloc(n * sizeof(float));
	float* z=(float *)malloc(n*sizeof(float));

	for (ip = 0; ip < n; ip++) {
		for (iq = 0; iq < n; iq++) v[ip][iq] = 0.0;
		v[ip][ip] = 1.0;
	}
	for (ip = 0; ip < n; ip++) {
		b[ip] = d[ip] = a[ip][ip];
		z[ip] = 0.0;
	}
	nrot = 0;
	for (i = 1; i <= 50; i++) {
		sm = 0.0;
		for (ip = 0; ip < n - 1; ip++) {
			for (iq = ip + 1; iq < n; iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0)
			return;
		if (i < 4)
			tresh = 0.2*sm / (n*n);
		else
			tresh = 0.0;
		for (ip = 0; ip < n - 1; ip++) {
			for (iq = ip + 1; iq < n; iq++) {
				g = 100.0*fabs(a[ip][iq]);
				if (i > 4 && (fabs(d[ip]) + g) == fabs(d[ip])
					&& (fabs(d[iq]) + g) == fabs(d[iq]))
					a[ip][iq] = 0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h = d[iq] - d[ip];
					if ((fabs(h) + g) == fabs(h))
						t = (a[ip][iq]) / h;
					else {
						theta = 0.5*h / (a[ip][iq]);
						t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
						if (theta < 0.0) t = -t;
					}
					c = 1.0 / sqrt(1 + t * t);
					s = t * c;
					tau = s / (1.0 + c);
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.0;
					for (j = 0; j < ip; j++)
						rot(a, s, tau, j, ip, j, iq);
					for (j = ip + 1; j < iq; j++)
						rot(a, s, tau, ip, j, j, iq);
					for (j = iq + 1; j < n; j++)
						rot(a, s, tau, ip, j, iq, j);
					for (j = 0; j < n; j++)
						rot(v, s, tau, j, ip, j, iq);
					++nrot;
				}
			}
		}
		for (ip = 0; ip < n; ip++) {
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.0;
		}
	}
	printf("Too many iterations in routine jacobi");
}

void eigsrt(float* d, float **v,int d_size)
{
	int i, j, k;
	float p;

	int n = d_size;
	for (i = 0; i < n - 1; i++) {
		p = d[k = i];
		for (j = i; j < n; j++)
			if (d[j] >= p) p = d[k = j];
		if (k != i) {
			d[k] = d[i];
			d[i] = p;
			for (j = 0; j < n; j++) {
				p = v[j][i];
				v[j][i] = v[j][k];
				v[j][k] = p;
			}
		}
	}
}

float ran1(long long &idum)
{
	const int IA = 16807, IM = 2147483647, IQ = 127773, IR = 2836, NTAB = 32;
	const int NDIV = (1 + (IM - 1) / NTAB);
	const float EPS = 3.0e-16, AM = 1.0 / IM, RNMX = (1.0 - EPS);
	static int iy = 0;
	static float* iv = (float*)malloc(NTAB*sizeof(float));
	int j, k;
	float temp;

	if (idum <= 0 || !iy) {
		if (-idum < 1) idum = 1;
		else idum = -idum;
		for (j = NTAB + 7; j >= 0; j--) {
			k = idum / IQ;
			idum = IA * (idum - k * IQ) - IR * k;
			if (idum < 0) idum += IM;
			if (j < NTAB) iv[j] = idum;
		}
		iy = iv[0];
	}
	k = idum / IQ;
	idum = IA * (idum - k * IQ) - IR * k;
	if (idum < 0) idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = idum;
	if ((temp = AM * iy) > RNMX) return RNMX;
	else return temp;
}


float gasdev(long long  &idum)
{
	static int iset = 0;
	static float gset;
	float fac, rsq, v1, v2;

	if (idum < 0) iset = 0;
	if (iset == 0) {
		do {
			v1 = 2.0*ran1(idum) - 1.0;
			v2 = 2.0*ran1(idum) - 1.0;
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0*log(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	}
	else {
		iset = 0;
		return gset;
	}
}


int main() {
	const int N = 11;
	float **a = (float**)malloc((N+1) * sizeof(float*));
	float **v = (float**)malloc((N+1) * sizeof(float*));
	float *d = (float*)malloc((N+1) * sizeof(float));

	for (int i = 0; i <= N; i++)
	{
		a[i] = (float*)malloc((N+1) * sizeof(float));
		v[i] = (float*)malloc((N+1) * sizeof(float));
	}

	long long idum = time(NULL);
	int nrot;

	printf("Generate random matrix....\n");
	for (int i = 1; i <= N; i++)
		for (int j = i; j <= N; j++) {
			float ran = gasdev(idum);
			a[i][j] = ran;
			a[j][i] = ran;
		}

	printf("Random matrix before Succesive diagonalization:\n");
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			printf("%f ", a[i][j]);
		}
		printf("\n");
	}
	
	jacobi(a, d, v, nrot, N+1);
	eigsrt(d, v, N+1);

	printf("\nRandom matrix after Succesive diagonalization:\n");
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			printf("%f ", a[i][j]);
		}
		printf("\n");
	}


	printf("\nEigen vectors:\n");
	for (int i = 1; i <= N; i++ ) {
		for (int j = 1; j <= N; j++) {
			printf("%f ", v[i][j]);
		}
		printf("\n");
	}

	printf("\nEigen values:\n");
	for (int i = 1; i <= N; i++)
		printf("%f ", d[i]);

	return 0;
}