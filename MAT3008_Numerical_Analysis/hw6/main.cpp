#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <cmath>

float ran1(long long  &idum)
{
	const int IA = 16807, IM = 2147483647, IQ = 127773, IR = 2836, NTAB = 32;
	const int NDIV = (1 + (IM - 1) / NTAB);
	const float EPS = 3.0e-16, AM = 1.0 / IM, RNMX = (1.0 - EPS);
	static int iy = 0;
	static int iv[NTAB];
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


int main(int argc, char *argv[]) {
	int n = atoi(argv[1]);
	char* method = argv[2];
	long long seed;



	seed = time(NULL);
	if (strcmp(method, "uniform") == 0) {
		for (int i = 0; i < n; i++) {
			printf("%f ", 5 * ran1(seed) - 3);
		}
	}
	else if (strcmp(method, "gaussian") == 0) {
		for (int i = 0; i < n; i++) {
			printf("%f ", 1.5*gasdev(seed) + 0.5);

		}

	}
}