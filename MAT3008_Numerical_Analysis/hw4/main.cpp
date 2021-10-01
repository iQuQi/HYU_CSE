
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <time.h>
#include <cmath>
#include "rt.cpp"

using namespace NR;

//Bisection / Linear Method 용
DP f1(DP R) {
	return exp(-0.005 * R) * cos(sqrt(2000 - R * R * 0.01) * 0.05) - 0.01;
}

//Newton / Secant Method 용 - 미분
void df1(DP R, DP &y, DP &dy) {
	y = f1(R);
	float tmp = sqrt(2000 - R * R * 0.01) * 0.05;
	dy = exp(-0.005 * R) * (sin(tmp) / tmp - cos(tmp)) * 0.005;
}

void problem1() {
	printf("[Problem 1]=====================================================\n");
	rtbis_method(f1, 0, 400, 1e-4);
	rtflsp_method(f1, 0, 400, 1e-4);
	rtsec_method(f1, 0, 400, 1e-4);
	rtnewt_method(df1, 0, 400, 1e-4);
	rtsafe_method(df1, 0, 400, 1e-4);
	printf("----------------------------------------------------------------\n");
	rtbis_method(f1, 0, 400, 1e-6);
	rtflsp_method(f1, 0, 400, 1e-6);
	rtsec_method(f1, 0, 400, 1e-6);
	rtnewt_method(df1, 0, 400, 1e-6);
	rtsafe_method(df1, 0, 400, 1e-6);
}

//Bisection / Linear Method 용
DP f2(DP R) {
	return pow(R*R + .9*.9, 1.5)*(8.85 * M_PI) / (100) - R;
}

//Newton / Secant Method 용 - 미분
void df2(DP R, DP &y, DP &dy) {
	y = f2(R);
	dy = (531 * M_PI* R*sqrt(100 * pow(R,2)+ 81) - 20000) / 20000;
}

void problem2() {
	printf("[Problem 2]=====================================================\n");
	rtbis_method(f2, 0, 2, 1e-4);
	rtflsp_method(f2, 0, 2, 1e-4);
	rtsec_method(f2, 0, 2, 1e-4);
	rtnewt_method(df2, 0, 2, 1e-4);
	rtsafe_method(df2, 0, 2, 1e-4);
}

//Bisection / Linear Method 용
DP f3(DP R) {
	return -0.20597 + 1.671e-4 * R + 9.7215e-8 * pow(R, 2) - 9.5838e-11 * pow(R, 3) + 1.9520e-14 * pow(R, 4);
}

//Newton / Secant Method 용 - 미분
void df3(DP R, DP &y, DP &dy) {
	y = f3(R);
	dy = 1.671e-4 + 9.7215e-8 * 2 * R - 9.5838e-11 * 3 * pow(R, 2) + 1.9520e-14 * 4 * pow(R, 3);
}

void problem3() {
	printf("[Problem 3]=====================================================\n");
	rtbis_method(f3, 1000, 1500, 1e-4);
	rtflsp_method(f3, 1000, 1500, 1e-4);
	rtsec_method(f3, 1000, 1500, 1e-4);
	rtnewt_method(df3, 1000, 1500, 1e-4);
	rtsafe_method(df3, 1000, 1500, 1e-4);
}


int main() {
	problem1();
	problem2();
	problem3();

	return 0;
}