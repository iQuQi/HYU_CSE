#include <cmath>
#include "nr.h"
using namespace std;


//Bisection Method
DP rtbis(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{// f(x)/��������/������/����� ����/�ݺ� Ƚ��
	const int JMAX = 40;
	int j;
	DP dx, f, fmid, xmid, rtb;

	f = func(x1);//���� �������� �־��
	fmid = func(x2); // ���� ������ �־� ����
	if (f*fmid >= 0.0) printf("Root must be bracketed for bisection in rtbis\n");

	//rtb�� ������ ������ , dx�� ������ ũ��
	rtb = f < 0.0 ? (dx = x2 - x1, x1) : (dx = x1 - x2, x2);
	/*
	rtb = dx = x2 - x1 ;
	rtb = x1 ;
	�� rtb�� ��������� dx�� ������ �Ǵ� ��Ȳ
	*/

	//Max Ž�� �� ��ŭ ���ݾ� �ٿ����鼭 ã�´�.
	for (j = 0; j < JMAX; j++) {

		//�������� ������ �߰� ���� y���� ���Ѵ�.
		fmid = func(xmid = rtb + (dx *= 0.5));

		//�ش� ���� 0���� �۰ų� ���ٸ� rtb���� ���� 
		//�ƴ϶�� �״�� �� -> ���� ���ʿ� dx�� ������ �ٿ��� �˻�
		if (fmid <= 0.0) rtb = xmid;

		//��밡���� ������ �Ǿ��ų� ��Ʈ�� �߰��ϸ� ��
		if (fabs(dx) < xacc || fmid == 0.0) {
			*num_iter = j + 1;
			return rtb;
		}
	}

	//Max Ž�� �� �ʰ�
	printf("Too many bisections in rtbis\n");
	return 0.0;
}

//Linear interpolation Method
DP rtflsp(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{
	const int MAXIT = 30;
	int j;
	DP fl, fh, xl, xh, dx, del, f, rtf;

	//������ ���۰� ������ �־��
	fl = func(x1);
	fh = func(x2);
	if (fl*fh > 0.0) printf("Root must be bracketed in rtflsp\n");

	//�Լ����� ���������� xl���� �д�
	//���� ���� ����
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {//������ ���� ����
		xl = x2;
		xh = x1;
		SWAP(fl, fh);
	}

	//Max Ž�� �� ��ŭ ã�´�
	//������ �糡���� ��� ���⸦ Ȱ�� -> ������ �ٿ�����
	dx = xh - xl;//������ ũ��
	for (j = 0; j < MAXIT; j++) {
		//Pn+1 (���� ��)
		rtf = xl + dx * fl / (fl - fh);
		f = func(rtf);

		//f���� ������ �� x1 �� ������ ������ ��
		if (f < 0.0) {
			del = xl - rtf;
			xl = rtf;
			fl = f;
		}
		//f���� ����� �� x2 �� ������ ������ ��
		else {
			del = xh - rtf;
			xh = rtf;
			fh = f;
		}

		//���ο� ������ ũ�� ����
		dx = xh - xl;

		//��밡���� ������ �Ǿ��ų� ��Ʈ�� �߰��ϸ� ��
		if (fabs(del) < xacc || f == 0.0) {
			*num_iter = j + 1;
			return rtf;
		}
	}

	//Max Ž�� �� �ʰ�
	printf("Maximum number of iterations exceeded in rtflsp\n");
	return 0.0;
}


//Secant Method
DP rtsec(DP func(const DP), const DP x1, const DP x2, const DP xacc, int *num_iter)
{
	const int MAXIT = 30;
	int j;
	DP fl, f, dx, xl, rts;

	//�糡���� �Լ��� Ȯ��
	fl = func(x1);
	f = func(x2);

	//�Լ����� ���� �� -> ������ ū���� xl(left)�� �д� & rts�� x2(right)���� ����
	//xl�� rts�� pn �� pn-1�� �ش�
	if (fabs(fl) < fabs(f)) {
		rts = x1;
		xl = x2;
		SWAP(fl, f);
	}
	else {
		xl = x1;
		rts = x2;
	}

	//Max ��ŭ Ž�� �ϱ�
	for (j = 0; j < MAXIT; j++) {
		//Pn+1 - Pn �� ����
		dx = (xl - rts)*f / (f - fl);

		xl = rts; //������ right�� left�� ����
		fl = f; // x1�� ����ʿ� ���� f1�� ��������
		rts += dx; // Pn�� dx�� ���ؼ� Pn+1((���ο� right)�� ���Ѵ�
		f = func(rts); //���ο� Pn(right)���� f ����

		//��밡���� ������ �Ǿ��ų� ��Ʈ�� �߰��ϸ� ��
		if (fabs(dx) < xacc || f == 0.0) {
			*num_iter = j + 1;
			return rts;
		}
	}

	//Max Ž�� �� �ʰ�
	printf("Maximum number of iterations exceeded in rtsec\n");
	return 0.0;
}

//Newton-Raphson Method
DP rtnewt(void funcd(const DP, DP &, DP &), const DP x1, const DP x2,
	const DP xacc, int* num_iter)
{
	//������ �߰� �� ������ y=0�� ������ ���� ���� ���� �Ǵ� ���
	const int JMAX = 20;
	int j;
	DP df, dx, f, rtn;
	//f�� �Լ��� df �� �̺а�

	//�ʱ� �� P0 -> �߾Ӱ� ���
	rtn = 0.5*(x1 + x2);

	//Max ��ŭ Ž��
	for (j = 0; j < JMAX; j++) {
		funcd(rtn, f, df); //�Լ����� �̺а��� ���ؿ´�
		dx = f / df; 
		rtn -= dx; // Pn+1 = Pn -  f/df

		//�������� ��� -> �߻�?
		if ((x1 - rtn)*(rtn - x2) < 0.0)
			printf("Jumped out of brackets in rtnewt\n");

		//��밡���� ������ �Ǿ��ٸ�  ��
		if (fabs(dx) < xacc) {
			*num_iter = j + 1;
			return rtn;
		}
	}

	//Max Ž�� �� �ʰ�
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

	//�糡���� �Լ����� �̺а��� �˾ƿ´�
	funcd(x1, fl, df);
	funcd(x2, fh, df);


	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0))
		printf("Root must be bracketed in rtsafe\n");

	//�ٷ� ��Ʈ�� ã�Ҵٸ� �״�� ����
	if (fl == 0.0) return x1;
	if (fh == 0.0) return x2;

	//�Լ����� ������ ���� left�� �д�
	if (fl < 0.0) {
		xl = x1;
		xh = x2;
	}
	else {
		xh = x1;
		xl = x2;
	}

	//�߾Ӱ��� P0�� ��´�
	rts = 0.5*(x1 + x2);
	//������ ����
	dxold = fabs(x2 - x1); 
	dx = dxold;
	//�߾Ӱ��� �Լ����� �̺а� �˾ƿ���
	funcd(rts, f, df);

	//Max ��ŭ Ž��
	for (j = 0; j < MAXIT; j++) {
		//
		if ((((rts - xh)*df - f)*((rts - xl)*df - f) > 0.0)
			|| (fabs(2.0*f) > fabs(dxold*df))) { 
			// ������Ʈ �� x�� ������ ����ų� �ʹ� ���� ������Ʈ �Ǹ� Bisection ������� update
			dxold = dx;		// ���� dx ����
			dx = 0.5*(xh - xl);//�ش� ������ ���� -> �� dx
			rts = xl + dx; 
			//������ 0�� �Ǹ�?..
			if (xl == rts) {
				*num_iter = j + 1;
				return rts;
			}
		}
		//�ƴ϶�� �״�� Newton ������� update
		else {
			dxold = dx;  // ���� dx ����
			dx = f / df; // �� dx
			temp = rts; // Pn ����
			rts -= dx; // Pn+1
			//Pn�� Pn+1�� ���ٸ�?..
			if (temp == rts) {
				*num_iter = j + 1;
				return rts;
			}
		}

		//��밡���� ������ �Ǿ��ٸ� ��
		if (fabs(dx) < xacc) {
			*num_iter = j + 1;
			return rts;
		}

		//Pn+1�� ���� �Լ����� �̺а� ���ϱ�
		funcd(rts, f, df);
		//Pn+1 �� �Լ��� ��ȣ�� ���� ���� �� ����
		if (f < 0.0)
			xl = rts;
		else
			xh = rts;
	}

	//Max Ž�� �� �ʰ�
	printf("Maximum number of iterations exceeded in rtsafe\n");
	return 0.0;
}


//���� ����� ����ؼ� ��Ʈ �������� ���ϰ� Iteration �� ���ϱ�
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