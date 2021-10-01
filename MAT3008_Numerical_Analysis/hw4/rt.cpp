#include <cmath>
#include "nr.h"
using namespace std;


//Bisection Method
DP rtbis(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{// f(x)/범위시작/범위끝/상대적 오차/반복 횟수
	const int JMAX = 40;
	int j;
	DP dx, f, fmid, xmid, rtb;

	f = func(x1);//범위 시작점을 넣어본다
	fmid = func(x2); // 범위 끝점을 넣어 본다
	if (f*fmid >= 0.0) printf("Root must be bracketed for bisection in rtbis\n");

	//rtb는 범위의 시작점 , dx는 범위의 크기
	rtb = f < 0.0 ? (dx = x2 - x1, x1) : (dx = x1 - x2, x2);
	/*
	rtb = dx = x2 - x1 ;
	rtb = x1 ;
	로 rtb는 덮어씌워지고 dx는 대입이 되는 상황
	*/

	//Max 탐색 수 만큼 절반씩 줄여가면서 찾는다.
	for (j = 0; j < JMAX; j++) {

		//절반으로 나눠서 중간 값의 y값을 구한다.
		fmid = func(xmid = rtb + (dx *= 0.5));

		//해당 값이 0보다 작거나 같다면 rtb값을 갱신 
		//아니라면 그대로 둠 -> 다음 차례에 dx를 반으로 줄여서 검사
		if (fmid <= 0.0) rtb = xmid;

		//허용가능한 오차가 되었거나 루트를 발견하면 끝
		if (fabs(dx) < xacc || fmid == 0.0) {
			*num_iter = j + 1;
			return rtb;
		}
	}

	//Max 탐색 수 초과
	printf("Too many bisections in rtbis\n");
	return 0.0;
}

//Linear interpolation Method
DP rtflsp(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{
	const int MAXIT = 30;
	int j;
	DP fl, fh, xl, xh, dx, del, f, rtf;

	//범위의 시작과 끝점을 넣어본다
	fl = func(x1);
	fh = func(x2);
	if (fl*fh > 0.0) printf("Root must be bracketed in rtflsp\n");

	//함수값이 음수인쪽을 xl으로 둔다
	//왼쪽 점이 음수
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {//오른쪽 점이 음수
		xl = x2;
		xh = x1;
		SWAP(fl, fh);
	}

	//Max 탐색 수 만큼 찾는다
	//구간의 양끝점의 평균 기울기를 활용 -> 구간을 줄여나감
	dx = xh - xl;//범위의 크기
	for (j = 0; j < MAXIT; j++) {
		//Pn+1 (다음 항)
		rtf = xl + dx * fl / (fl - fh);
		f = func(rtf);

		//f값이 음수일 때 x1 쪽 구간을 버려야 함
		if (f < 0.0) {
			del = xl - rtf;
			xl = rtf;
			fl = f;
		}
		//f값이 양수일 때 x2 쪽 구간을 버려야 함
		else {
			del = xh - rtf;
			xh = rtf;
			fh = f;
		}

		//새로운 범위의 크기 갱신
		dx = xh - xl;

		//허용가능한 오차가 되었거나 루트를 발견하면 끝
		if (fabs(del) < xacc || f == 0.0) {
			*num_iter = j + 1;
			return rtf;
		}
	}

	//Max 탐색 수 초과
	printf("Maximum number of iterations exceeded in rtflsp\n");
	return 0.0;
}


//Secant Method
DP rtsec(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{
	const int MAXIT = 30;
	int j;
	DP fl, f, dx, xl, rts;

	//양끝점의 함수값 확인
	fl = func(x1);
	f = func(x2);

	//함수값의 절댓값 비교 -> 절댓값이 큰쪽을 xl(left)로 둔다 & rts에 x2(right)값을 대입
	//xl과 rts가 pn 과 pn-1에 해당
	if (fabs(fl) < fabs(f)) {
		rts = x1;
		xl = x2;
		SWAP(fl, f);
	}
	else {
		xl = x1;
		rts = x2;
	}

	//Max 만큼 탐색 하기
	for (j = 0; j < MAXIT; j++) {
		//Pn+1 - Pn 의 길이
		dx = (xl - rts)*f / (f - fl);

		xl = rts; //기존의 right를 left로 변경
		fl = f; // x1이 변경됨에 따라 f1도 변경해줌
		rts += dx; // Pn에 dx를 더해서 Pn+1((새로운 right)을 구한다
		f = func(rts); //새로운 Pn(right)으로 f 변경

		//허용가능한 오차가 되었거나 루트를 발견하면 끝
		if (fabs(dx) < xacc || f == 0.0) {
			*num_iter = j + 1;
			return rts;
		}
	}

	//Max 탐색 수 초과
	printf("Maximum number of iterations exceeded in rtsec\n");
	return 0.0;
}

//Newton-Raphson Method
DP rtnewt(void funcd(const DP, DP &, DP &), const DP x1, const DP x2,
	const DP xacc, int* num_iter)
{
	//접선을 긋고 그 접선이 y=0과 만나는 점이 다음 점이 되는 방식
	const int JMAX = 20;
	int j;
	DP df, dx, f, rtn;
	//f는 함수값 df 는 미분값

	//초기 값 P0 -> 중앙값 사용
	rtn = 0.5*(x1 + x2);

	//Max 만큼 탐색
	for (j = 0; j < JMAX; j++) {
		funcd(rtn, f, df); //함수값과 미분값을 구해온다
		dx = f / df; 
		rtn -= dx; // Pn+1 = Pn -  f/df

		//범위에서 벗어남 -> 발산?
		if ((x1 - rtn)*(rtn - x2) < 0.0)
			printf("Jumped out of brackets in rtnewt\n");

		//허용가능한 오차가 되었다면  끝
		if (fabs(dx) < xacc) {
			*num_iter = j + 1;
			return rtn;
		}
	}

	//Max 탐색 수 초과
	printf("Maximum number of iterations exceeded in rtnewt\n");
	return 0.0;
}


//Newton with Bracking Method
DP rtsafe(void funcd(const DP, DP &, DP &), const DP x1, const DP x2,
	const DP xacc, int *num_iter)
{
	const int MAXIT = 100;
	int j;
	DP df, dx, dxold, f, fh, fl, temp, xh, xl, rts;

	//양끝점의 함수값과 미분값을 알아온다
	funcd(x1, fl, df);
	funcd(x2, fh, df);


	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0))
		printf("Root must be bracketed in rtsafe\n");

	//바로 루트를 찾았다면 그대로 리턴
	if (fl == 0.0) return x1;
	if (fh == 0.0) return x2;

	//함수값이 음수인 쪽을 left로 둔다
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {
		xh = x1;
		xl = x2;
	}

	//중앙값을 P0로 잡는다
	rts = 0.5*(x1 + x2);
	//구간의 길이
	dxold = fabs(x2 - x1); 
	dx = dxold;
	//중앙값의 함수값과 미분값 알아오기
	funcd(rts, f, df);

	//Max 만큼 탐색
	for (j = 0; j < MAXIT; j++) {
		//
		if ((((rts - xh)*df - f)*((rts - xl)*df - f) > 0.0)
			|| (fabs(2.0*f) > fabs(dxold*df))) { 
			// 업데이트 된 x가 범위를 벗어나거나 너무 조금 업데이트 되면 Bisection 방식으로 update
			dxold = dx;		// 원래 dx 저장
			dx = 0.5*(xh - xl);//해당 범위의 절반 -> 새 dx
			rts = xl + dx; 
			//범위가 0이 되면?..
			if (xl == rts) {
				*num_iter = j + 1;
				return rts;
			}
		}
		//아니라면 그대로 Newton 방식으로 update
		else {
			dxold = dx;  // 원래 dx 저장
			dx = f / df; // 새 dx
			temp = rts; // Pn 저장
			rts -= dx; // Pn+1
			//Pn이 Pn+1과 같다면?..
			if (temp == rts) {
				*num_iter = j + 1;
				return rts;
			}
		}

		//허용가능한 오차가 되었다면 끝
		if (fabs(dx) < xacc) {
			*num_iter = j + 1;
			return rts;
		}

		//Pn+1의 값의 함수값과 미분값 구하기
		funcd(rts, f, df);
		//Pn+1 의 함수값 부호에 따라 구간 재 설정
		if (f < 0.0)
			xl = rts;
		else
			xh = rts;
	}

	//Max 탐색 수 초과
	printf("Maximum number of iterations exceeded in rtsafe\n");
	return 0.0;
}


//위의 방법을 사용해서 루트 추정값을 구하고 Iteration 을 구하기
void rtbis_method(DP(*f)(DP), DP x1, DP x2, DP xacc) {
	int num_iter = 0;
	printf("[rtbis] : range [%.1e, %.1e], accuracy %.1e\n", x1, x2, xacc);
	printf("Estimated value: %.10f\n", rtbis(f, x1, x2, xacc, &num_iter));
	printf("Iterations: %d\n\n", num_iter);
}

void rtflsp_method(DP(*f)(DP), DP x1, DP x2, DP xacc) {
	int num_iter = 0;
	printf("[rtflsp] : range [%.1e, %.1e], accuracy %.1e\n", x1, x2, xacc);
	printf("Estimated value: %.10f\n", rtflsp(f, x1, x2, xacc, &num_iter));
	printf("Iterations: %d\n\n", num_iter);
}

void rtsec_method(DP(*f)(DP), DP x1, DP x2, DP xacc) {
	int num_iter = 0;
	printf("[rtsec] : range [%.1e, %.1e], accuracy %.1e\n", x1, x2, xacc);
	printf("Estimated value: %.10f\n", rtsec(f, x1, x2, xacc, &num_iter));
	printf("Iterations: %d\n\n", num_iter);
}

void rtnewt_method(void(*df)(DP, DP &, DP &), DP x1, DP x2, DP xacc) {
	int num_iter = 0;
	printf("[rtnewt] ; range [%.1e, %.1e], accuracy %.1e\n", x1, x2, xacc);
	printf("Estimated value: %.10f\n", rtnewt(df, x1, x2, xacc, &num_iter));
	printf("Iterations: %d\n\n", num_iter);
}

void rtsafe_method(void(*df)(DP, DP &, DP &), DP x1, DP x2, DP xacc) {
	int num_iter = 0;
	printf("[rtsafe] : range [%.1e, %.1e], accuracy %.1e\n", x1, x2, xacc);
	printf("Estimated value: %.10f\n", rtsafe(df, x1, x2, xacc, &num_iter));
	printf("Iterations: %d\n\n", num_iter);
}