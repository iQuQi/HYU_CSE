
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "rtbis.cpp"
#include "rtflsp.cpp"
#include "rtnewt.cpp"
#include "rtsafe.cpp"
#include "rtsec.cpp"

#include "bessj0.cpp"
#include "bessj1.cpp"
using namespace NR;
typedef float DP;


//아래의 두 함수는 타입에 맞는 함수를 받아와서 root를 찾게 한 후
//걸린 시간과 root값을 구하는 것이다.
void run_and_clock_1(DP(*method)(DP(*func)(DP), DP, DP, DP),
	DP(*func)(DP),
	DP *xb1,DP *xb2,int num_roots,DP xacc) {
	time_t start, end, total;
	total = 0;
	for (int i = 1; i <= num_roots; i++) {
		start = (double)clock() ;
		DP root = (*method)(func, xb1[i], xb2[i], xacc);
		end = (double)clock() ;
		total += end - start;
		printf("[%d]: %.10f\n", i, root);
	}

	printf("Time: %ldus\n\n", total* (int)1e6 / CLOCKS_PER_SEC);
}

void run_and_clock_2(DP(*method)(void(*func)(DP, DP &, DP &), DP, DP, DP),
	void(*func)(DP, DP &, DP &),
	DP *xb1,DP *xb2,int num_roots,DP xacc) {
	time_t start, end, total;
	total = 0;
	for (int i = 1; i <= num_roots; i++) {
		start = (double)clock();
		DP root = (*method)(func, xb1[i], xb2[i], xacc);
		end = (double)clock();
		total += end - start;
		printf("[%d]: %.10f\n", i, root);
	}
	printf("Time: %ldus\n\n", total * (int)1e6 / CLOCKS_PER_SEC);
}
/////////////////////////////////////

void bessj0d(DP x, DP &y, DP &dy) {
	y = bessj0(x);
	dy = -bessj1(x);
}

//zbrak 함수 재정의
void zbrak(DP(*fx)(DP), DP x1, DP x2, int n, DP xb1[],
	DP xb2[], int *nb)
{
	int nbb, i;
	DP x, fp, fc, dx;

	nbb = 0;
	dx = (x2 - x1) / n;
	fp = (*fx)(x = x1);
	for (i = 1; i <= n; i++) {
		fc = (*fx)(x += dx);
		if (fc*fp <= 0.0) {
			xb1[++nbb] = x - dx;
			xb2[nbb] = x;
			if (*nb == nbb) return;

		}
		fp = fc;
	}
	*nb = nbb;
}


void problem_0() {
	printf("[ Problem 0 ]\n");

	//구간과 상대오차 설정
	const DP X1 = 1.0;
	const DP X2 = 10.0;
	const DP XACC = 1e-6;

	const int MAX_ROOTS = 100;
	const int NUM_BRACKETS = 1e5;

	int num_roots = 100;
	DP xb1[MAX_ROOTS], xb2[MAX_ROOTS];
	//Bracketing 방식 : 답이 존재한다는 것을 알고 있는 구간의 범위를 줄여나감
	zbrak(bessj0, X1, X2, NUM_BRACKETS, xb1, xb2, &num_roots);

	//아래는 순서대로 다른 방식을 적용해서 root를 찾는 과정
	printf("---------Bisection method---------\n");
	run_and_clock_1(rtbis, bessj0, xb1, xb2, num_roots, XACC);

	printf("---------Linear interpolation---------\n");
	run_and_clock_1(rtflsp, bessj0, xb1, xb2, num_roots, XACC);

	printf("---------Secant method---------\n");
	run_and_clock_1(rtsec, bessj0, xb1, xb2, num_roots, XACC);

	//뮬러 방식은 구현하지 못하였습니다..
	//printf("---------Muller method---------\n");
	//run_and_clock_1(muller, bessj0, xb1, xb2, num_roots, XACC);

	printf("---------Newton-Raphson method---------\n");
	run_and_clock_2(rtnewt, bessj0d, xb1, xb2, num_roots, XACC);

	printf("---------Newton with bracketing---------\n");
	run_and_clock_2(rtsafe, bessj0d, xb1, xb2, num_roots, XACC);

}
///////////////////////////////////////////

//주어진 문제식
DP func_1(DP x) {
	return 10 * exp(-x) * sin(2 * M_PI * x) - 2;
}

void func_1d(DP x, DP &y, DP &dy) {
	y = func_1(x);
	dy = 10 * exp(-x) * (2 * M_PI * cos(2 * M_PI * x) - sin(2 * M_PI * x));
}

void problem_1() {
	printf("[ Problem 1 ]\n");

	//주어진 구간과 상대 오차 변화 값
	const DP X1 = 0.1;
	const DP X2 = 1;
	const DP XACC = 1e-6;

	//루트 개수와 구간 설정
	int num_roots = 1;
	DP xb1[2] = { 0, X1 };
	DP xb2[2] = { 0, X2 };

	run_and_clock_2(rtsafe, func_1d, xb1, xb2, num_roots, XACC);
}

//////////////////////////////////////////

//주어진 문제식
DP func_2(DP x) {
	return exp(x) * x - 1;
}

void func_2d(DP x, DP &y, DP &dy) {
	y = func_2(x);
	dy = exp(x) * (x + 1);
}

void problem_2() {
	printf("[ Problem 2 ]\n");

	//주어진 구간과 상대 오차 변화 값
	const DP X1 = 0;
	const DP X2 = 1;
	const DP XACC = 1e-6;

	//루트 개수와 구간 설정
	int num_roots = 1;
	DP xb1[2] = { 0, X1 };
	DP xb2[2] = { 0, X2 };

	run_and_clock_2(rtsafe, func_2d, xb1, xb2, num_roots, XACC);
}

/////////////////////////////////////////////

//주어진 문제식
DP func_3(DP x) {
	return cos(x + sqrt(2)) + x * (x / 2 + sqrt(2));
}

void func_3d(DP x, DP &y, DP &dy) {
	y = func_3(x);
	dy = x - sin(x + sqrt(2)) + sqrt(2);
}

void problem_3() {
	printf("[ Problem 3 ]\n");

	//주어진 구간과 상대 오차 변화 값
	const DP X1 = -2;
	const DP X2 = -1;
	const DP XACC = 1e-6;

	//루트 개수와 구간 설정
	int num_roots = 1;
	DP xb1[2] = { 0, X1 };
	DP xb2[2] = { 0, X2 };

	run_and_clock_2(rtsafe, func_3d, xb1, xb2, num_roots, XACC);
}

int main() {
	//여러 방법으로 root 구해보기
	problem_0();
	
	//주어진 식에서 root 찾기
	problem_1();
	problem_2();
	problem_3();

	return 0;
}