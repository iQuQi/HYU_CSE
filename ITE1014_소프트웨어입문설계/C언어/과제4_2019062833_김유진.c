#include <stdio.h>
#include <stdlib.h>
//먼저 문자열배열을 크기를 지정해주며 선언해준다
char fir[30], sec[30],menu[5];

//더하기함수
float add(float a, float b) {
	float result;
	result = a + b;
	return result;

}
//빼기함수
float sub(float a, float b) {
	float result;
	result = a - b;
	return result;

}
//곱하기 함수
float mul(float a, float b) {
	float result;
	result = a * b;
	return result;
}
//나누기함수
float di(float a, float b) {
	float result;
	result = a / b;
	return result;
}
//메인함수 시작
int main() {
	
	//사용할 정수와 실수들을 선언
	int menu2=10,fir2,sec2,count=0;
	float re,result;
	
	//처음 계산기 화면 출력
	printf("------------------------\n    Last calculator\n------------------------\n");
	
	//메뉴를 계속 입력 받아야하므로 while문 사용
	while (1) {

		//루프가 몇번 돌았는 지 카운트
		count++;
		
		
		
		//메뉴출력
		printf("1. add\n2. sub\n3. mul\n4. div\n0 exit\n");
		printf("------------------------\n");
		//사용자에게 메뉴 입력받기
		while (1) {
			scanf(" %c", menu);
			menu2 = atoi(menu);
			//문자를 입력 받았을 경우 다시 받게함
			if (menu2 == 0 && menu[0]!='0') {
					
				printf("wrong input!! input again\n");
				continue;
			}
			//메뉴에있지않은 수를 입력할 시 오류메세지를 띄운다
			else if(menu2!=1 && menu2!=2 && menu2!=3 && menu2!=4 && menu2!=0){
				

				printf("Wrong command!!!!!\n");
				continue;
			}
			//숫자를 재입력받았을 경우 while문 탈출
			else {
				break;
			}
		}
		
		
	
		//더하기함수를 입력받았을때
		if (menu2 == 1) {

			//처음실행일때
			if (count == 1) {
				//첫번째 숫자 입력받기
				printf("input first operand : ");
				//만약 입력받은것이 문자이면 다시 입력받는다
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				
				//두번째 숫자 입력받기
				printf("input second operand : ");
				//문자이면 다시 입력 받는다
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//더하기 함수를 호출하고 그 값을 프린트한다
				printf("result is %0.4f\n", add(fir2, sec2));
				//다음 연산에 넘겨줄 결과값을 변수에 넣어준다
				result = add(fir2, sec2);
			}
			//두번째 이상의 실행일 때 
			else {
				//전의 실행때 받아놓은 결과값을 프린트한다
				printf("%0.4f + ", result);
				//받아놓은 결과 값에 추가로 연산해줄 수를 입력받는다
				//문자를 입력받았을 경우 다시 입력받는다
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f +  ",result);
						continue;
					}
					else {
						break;
					}
				}
				//결과값과 새로받은 수를 연산해서 다시 다음 연산으로 넘겨줄 결과값으로 넣어준다
				printf("result is %0.4f\n", add(result, sec2));
				re = add(result, sec2);
				result = re;
			}
		}
		//빼기함수연산
		else if (menu2 == 2) {

			//처음실행일때
			if (count == 1) {
				printf("input first operand : ");
				//처음수를 입력받되 문자를 입력받을 시 다시 입력받는다
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//두번째 수를 입력받되 문자를 입력받을 시 다시 입력받는다
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//결과값을 프린트하고 다음 연산에 넘겨줄 변수에 저장한다
				printf("result is %0.4f\n", sub(fir2, sec2));
				result = sub(fir2, sec2);
			}
			//두번째 이상의 실행일때
			else {
				//앞선실행에서 받아놓은 결과값을 프린트한다
				printf("%0.4f - ", result);
				//연산할 새로운 수를 입력받되 문자를 입력 받을 시 다시 입력 받는다
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f - ",result);
						continue;
					}
					else {
						break;
					}
				}
				
				//넘겨받은 결과값과 새로받은 수를 연산하여 그 결과를 프린트하고 다음연산으로 넘겨줄 결과값을 변수에 저장
				printf("result is %0.4f\n", sub(result, sec2));
				re = sub(result, sec2);
				result = re;
			}

		}
		//곱하기연산실행
		else if (menu2 == 3) {

			//처음실행일때
			if (count == 1) {
				//첫번째 수를 입력받되 문자를 입력받을시 재입력받는다
				printf("input first operand : ");
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//두번째수를 입력받되 문자를 입력받을 시 재입력 받는다
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//결과값을 프린트하고 다음 연산에 넘겨줄 결과값을 저장한다
				printf("result is %0.4f\n", mul(fir2, sec2));
				result = mul(fir2, sec2);
			}
			//두번째 이상의 실행일때
			else {
				//넘겨받은 결과값 프린트
				printf("%0.4f * ", result);
				//새로운 수를 입력받되 문자를 입력받을 시 재입력받음
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again\n%0.4f * ",result);
						continue;
					}
					else {
						break;
					}
				}
				
				//넘겨받은 결과값과 새로받은수를 연산한뒤 그 결과값을 프린트하고 다음연산에 넘겨줄 결과값으로 저장한다
				printf("result is %0.4f\n", mul(result, sec2));
				re = mul(result, sec2);
				result = re;
			}
		}
		//나누기연산실행
		else if (menu2 == 4) {

			//처음실행일때
			if (count == 1) {
				//첫번째수를 입력받되 문자를 받으면 재입력받기
				printf("input first operand : ");
				while (1) {
					scanf("%s", fir);
					fir2 = atof(fir);
					if (fir2 == 0 && fir[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else {
						break;
					}
				}

				//두번째 수를 입력받되 문자를 받거나 0이면 다시 입력받기
				printf("input second operand : ");
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					else if (sec[0] == '0') {
						printf("can't divide by zero. input again : ");
						continue;
					}
					else {
						break;
					}
				}
				//두수를 연산한 뒤 그 결과값을 프린트하고 다음연산에 넘겨주기 위해 결과값으로 저장한다
				printf("result is %0.4f\n", di(fir2, sec2));
				result = di(fir2, sec2);

			}
			//두번째이상의 실행일때
			else {

				//앞의연산에서 받아온 결과값을 프린트하고 새로운수를 입력받되 문자이거나 0 이면 재입력받는다
				printf("%0.4f / ", result);
				while (1) {
					scanf("%s", sec);
					sec2 = atof(sec);
					if (sec[0] == '0') {
						printf("can't divide by zero. input again\n%0.4f / ",result);
						continue;
					}
					else if (sec2 == 0 && sec[0] != '0') {
						printf("wrong input!! input again : ");
						continue;
					}
					break;
				}
				
				//새로받은수와 앞에서 받아온 결과값을 연산한뒤 프린트하고 다시 결과값으로 저장해서 다음 연산으로 넘겨준다
				printf("result is %0.4f\n", di(result, sec2));
				re = di(result, sec2);
				result = re;
			}

		}
		//0을 입력받을 시 프로그램을 종료시킨다
		else if (menu2 == 0){
			printf("Good bye!!!");
			break;

		}
		

		

	}



	


}