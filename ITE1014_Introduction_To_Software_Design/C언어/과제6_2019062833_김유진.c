#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void printMenu();//메뉴를 프린트하는 함수
void print(int * arr, int size);//배열을 프린트해주는 함수
int * newTicket();//티켓 뽑아주는 함수
int* dangchum();//당첨 숫자 정해 놓는 함수
int soonwee2(int count, int count2);//순위 가르쳐주는 함수
int soonwee1(int* soonwee, int* dangchum, int** arr, int ticket, int cnt, int bu);//각각의 티켓 번호를 보여주고, 당첨된경우 몇등인지 가르쳐주며, 몇등을 몇번당첨됬는지 세어주는 함수


int main() {
	int menu, ticket;
	int totalpr = 0, totalbu = 0, earn, count = 0; // 총 금액, 총 티켓구매수, 번돈, 몇번째 구매인지
	char yn; //메뉴4에서 y or n 입력받을 변수
	int pr1 = 2000000000, pr2 = 100000000, pr3 = 2000000, pr4 = 50000, pr5 = 5000; // 당첨 금액
	int i, j;
	int **ticarr = NULL; //전체 티켓 목록을 넣어줄 이차원 배열
	int soonwee[5] = { 0,0,0,0,0 };//몇 등을 몇 번 당첨됬는지 넣어줄 배열
	int *dang = dangchum(); //당첨 숫자를 담은 배열

	srand(time(NULL));

	while (1) {

		//메뉴 프린트하고 입력받기
		printMenu();
		scanf("%d", &menu);

		count++;//몇 번째 구입인지 세주기

		//메뉴1 선택
		if (menu == 1) {

			//몇개의 티켓을 살지 입력받기
			printf("How many lines will you buy? : ");
			scanf("%d", &ticket);
			printf("\n");

			totalpr += 1000 * ticket;//총 지불 가격 더해주기
			totalbu += ticket;//총 구매티켓 개수 더해주기

			//처음 구매일때
			if (count == 1) {

				//이차원 배열 동적 할당
				ticarr = (int **)malloc(ticket * sizeof(int *));


				for (i = 0; i < ticket; i++) { // 구매한 개수만큼 티켓을 뽑아준다
					ticarr[i] = newTicket();
				}

				soonwee1(soonwee, dang, ticarr, ticket, count, totalbu);
			}

			//두번쨰 이상의 구매일때
			else {
				ticarr = (int**)realloc(ticarr, totalbu * sizeof(int*));// 총 티켓 구매 개수만큼 할당 추가로 시켜주기

				for (i = 0; i < ticket; i++) { // 구매한 개수만큼 티켓을 뽑아준다
					ticarr[totalbu - ticket + i] = newTicket();
				}

				soonwee1(soonwee, dang, ticarr, ticket, count, totalbu);
			}
		}

		//메뉴2선택
		else if (menu == 2) {
			printf("You bought %d tickets, and the total cost is %d won\n\n", totalbu, totalpr); // 총 티켓개수와 총 금액출력
		}

		//메뉴3 선택
		else if (menu == 3) {
			earn = soonwee[0] * pr1 + soonwee[1] * pr2 + soonwee[2] * pr3 + soonwee[3] * pr4 + soonwee[4] * pr5;//번 돈 계산
			printf("1st : %d 2nd : %d 3rd : %d 4th : %d 5th : %d\n", soonwee[0], soonwee[1], soonwee[2], soonwee[3], soonwee[4]);
			printf("You earned %d won\n\n", earn); // 번 돈 출력
		}

		//메뉴4 선택
		else if (menu == 4) {
			int menu4yn;//soonwee1함수에서 리턴된 yn값을 저장할 변수 선언
			int menu4pr, menu4ea;//메뉴4에서 쓴 지불금액,당첨금액
			int t;//메뉴4에서만 티켓 몇개샀는지 셀 때 사용

			//y or n 입력 받기
			printf("Are you sure? (y or n) : ");
			scanf("%s", &yn);

			//y를 입력했을 때
			if (yn == 'y') {
				//처음 구매일때
				if (count == 1) {

					//이차원 배열 동적 할당
					ticarr = (int **)malloc(sizeof(int *));

					menu4pr = 0;
					menu4ea = 0;
					t = 0;
					while (1) {
						
						int *ticarr2[1];//복사한 값을 넣어줄 배열 생성

						ticarr[t] = newTicket(); // 전체 티켓 목록에 추가해주기
						ticarr2[0] = ticarr[t]; // 하나씩 따로 soonwee1함수에 넣어주어야 하므로 값을 복사해둔다
						t++;
						ticarr = (int **)realloc(ticarr, (t + 1) * sizeof(int *));//이차원 배열을 한칸씩 while문이 돌때마다 늘려준다
						totalbu++; // 총 티켓 구매수 추가해주기
						totalpr += 1000; // 총 지불 가격에 추가해주기

						//티켓수는 한장씩 구매하므로 매개변수'ticket'자리에 1을 넣어준다
						menu4yn = soonwee1(soonwee, dang, ticarr2, 1, count, totalbu);
						menu4pr = 1000 * t;//메뉴4에서 지불한 가격을 계산해준다

						//받아온 yn의 값에 따라 당첨금액을 결정해준다
						if (menu4yn == 1) menu4ea = pr1;//1등
						if (menu4yn == 2) menu4ea = pr2;//2등
						if (menu4yn == 3) menu4ea = pr3;//3등
						if (menu4yn == 4) menu4ea = pr4;//4등


						if (menu4yn == 1||menu4yn==2) break; //soonwee1에서 리턴된 yn의 값이 1또는 2또는 3또는 4일 경우 4등이상이 당첨된 것이므로 while문 탈출

					}
					printf("total price for menu4 : %d\nyou earned %d\n", menu4pr, menu4ea);
				}

				//두번쨰 이상의 구매일때
				else {
					menu4pr = 0;
					menu4ea = 0;
					t = 0;
					while (1) {
						
						int *ticarr2[1];//복사한 값을 넣어줄 배열 생성

						ticarr = (int**)realloc(ticarr, (totalbu + 1) * sizeof(int*));// while문이 돌때 마다 하나씩 더 추가할당해준다

						ticarr[totalbu] = newTicket();// 전체 티켓 목록에 추가해주기
						ticarr2[0] = ticarr[totalbu];// 하나씩 따로 soonwee1함수에 넣어주어야 하므로 값을 복사해둔다
						totalbu++; // 총 티켓 구매수 추가
						totalpr += 1000;//총 지불 가격에 추가
						t+=1;

						//티켓수는 한장씩 구매하므로 매개변수 ticket자리에 1을 넣어주고, 
						//전체 티켓 목록이 아니라 하나뽑은 티켓 목록만 넘겨주므로 count값을 1로 넘겨주어  soonwee1함수에서 인덱스오류가 나지않도록한다
						menu4yn = soonwee1(soonwee, dang, ticarr2, 1, 1, totalbu);
						menu4pr = 1000 * t;//메뉴4에서 지불된 금액 계산

						//받아온 yn의 값에 따라 당첨된 금액을 결정해준다
						if (menu4yn == 1) menu4ea = pr1;//1등
						if (menu4yn == 2) menu4ea = pr2;//2등
						if (menu4yn == 3) menu4ea = pr3;//3등
						if (menu4yn == 4) menu4ea = pr4;//4등

						if (menu4yn == 1 || menu4yn == 2) break; //soonwee1에서 리턴된 yn의 값이 1또는 2또는 3또는 4일 경우 4등 이상이 당첨된 것이므로 while문 탈출

					}
					printf("total price for menu4 : %d\nyou earned %d\n", menu4pr, menu4ea);
				}
			}

			//y 이외의 것을 입력했을 때
			else {
				printf("\n");
				count--; // 구매 수에 영향안끼치게 한다
				continue;
			}
		}

		//메뉴0 선택
		else if (menu == 0) break;

		//이외의 숫자를 입력 했을 시 다시 입력 받기
		else {
			printf("Error : input again\n");
			count--; // 구매 수에 영향이 끼치지 않도록 한다
			continue;
		}
	}


	//메모리를 해제시켜준다
	for (i = 0; i < totalbu; ++i) {
		free(ticarr[i]);
	}
	free(ticarr);
}





///////////////////////////////////////////////////////////////////////////////




//메뉴를 프린트하는 함수
void printMenu() {
	printf("------------------Lottery Ticket------------------\n");
	printf("--------------------------------------------------\n1. Buy tickets\n2. Spent money\n3. Earned money\n4. Buy until I get 4th of higher prize\n0. exit\n");

}





////////////////////////////////////////////////////////////////////////////////





//배열을 프린트해주는 함수
void print(int * arr, int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("%d ", arr[i]);

	}
	printf("\n\n");
}





/////////////////////////////////////////////////////////////////////////////////





//티켓뽑아주는 함수
int * newTicket() {
	int i, j, temp, count = 0;
	int *ticketarr = NULL;

	ticketarr = (int*)malloc(6 * sizeof(int));

	//랜덤으로 뽑아주기
	for (i = 0; i < 6; ++i) {
		while (1) {
			ticketarr[i] = rand() % 45 + 1;
			for (j = 0; j < i; ++j) {//중복으로 안뽑히게 해줌
				if (ticketarr[i] == ticketarr[j]) count++;
			}
			if (count == 0) break;
			count = 0;

		}
	}

	//크기순으로 정렬
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5 - i; j++) {
			if (ticketarr[j] >= ticketarr[j + 1]) {
				temp = ticketarr[j];
				ticketarr[j] = ticketarr[j + 1];
				ticketarr[j + 1] = temp;
			}
		}
	}

	return ticketarr;

}






///////////////////////////////////////////////////////////////////////////////






//당첨숫자 정해놓는 함수
int* dangchum() {
	int i, bonus, j, temp, count = 0;
	int *dangchum = NULL;

	dangchum = (int*)malloc(7 * sizeof(int));

	//랜덤으로 뽑아주기
	for (i = 0; i < 6; ++i) {
		while (1) {
			dangchum[i] = rand() % 45 + 1;
			for (j = 0; j < i; ++j) { // 중복으로 안뽑히게 해줌
				if (dangchum[j] == dangchum[i]) count++;
			}
			if (count == 0) break;
			count = 0;

		}
	}

	//보너스 번호하나 뽑고 마지막 칸에 고정
	while (1) {
		count = 0;
		bonus = rand() % 45 + 1;
		for (j = 0; j < 6; ++j) { // 중복으로 안뽑히게 해줌
			if (dangchum[j] == bonus) count++;
		}
		if (count == 0) break;
	}
	dangchum[6] = bonus;


	//크기순으로 정렬- 마지막 보너스번호는 내버려둠
	for (i = 0; i < 5; ++i) {
		for (j = 0; j < 5 - i; ++j) {
			if (dangchum[j] > dangchum[j + 1]) {
				temp = dangchum[j];
				dangchum[j] = dangchum[j + 1];
				dangchum[j + 1] = temp;
			}

		}
	}

	return dangchum;
}






////////////////////////////////////////////////////////////////////////////






//순위가르쳐주는 함수
int soonwee2(int count, int count2) {
	int soonwee;
	if (count == 6) soonwee = 1;
	else if (count == 5 && count2 == 1) soonwee = 2;
	else if (count == 5 && count2 == 0) soonwee = 3;
	else if (count == 4) soonwee = 4;
	else if (count == 3) soonwee = 5;
	else return 0;

	return soonwee;
}






///////////////////////////////////////////////////////////////////////////






//각각의 티켓 번호를 보여주고, 당첨된 경우 몇등인지 가르쳐주며, 몇등을 몇번당첨됬는지 세어주는 함수
int soonwee1(int* soonwee, int* dangchum, int** arr, int ticket, int cnt, int bu) {
	int i, j, k;
	int count = 0, count2 = 0;//일반번호랑 몇개 일치하는지 세줄 변수, 보너스번호랑 일치하는지 세줄 변수
	int yn = 0; //메뉴4에서 당첨될때까지 받기위해 필요한 수 생성



	//처음 구매했을때
	if (cnt == 1) {

		for (i = 0; i < ticket; ++i) {
			printf("ticket [%d]\n", i + 1);//몇번째 티켓인지 프린트
			for (j = 0; j < 6; ++j) {
				printf("%d ", arr[i][j]); // 뽑은 번호를 출력
				for (k = 0; k < 6; k++) {
					if (arr[i][j] == dangchum[k]) {
						count++;//보너스번호가 아닌 일반번호와 몇개 일치하는지
					}
				}
				if (arr[i][j] == dangchum[6]) count2++;//보너스 번호랑 일치하는지

			}
			if (soonwee2(count, count2) != 0) {
				if (soonwee2(count, count2) ==1) yn=1;// 메뉴4에서 당첨될때까지 받기위해 필요한 수 만들기
				if (soonwee2(count, count2) == 2) yn = 2;
				if (soonwee2(count, count2) == 3) yn = 3;
				if (soonwee2(count, count2) == 4) yn = 4;
				if (soonwee2(count, count2) == 5) yn = 5;
				printf("ㅊㅋㅊㅋ %d등!!!!", soonwee2(count, count2));//당첨되면 알려준다
				soonwee[soonwee2(count, count2) - 1]++; // 몇등이 몇번당첨됬는지 세주기
			}
			printf("\n\n");

			count = 0;
			count2 = 0;
		}
	}




	//두번째 이상의 구매일때
	else {
		for (i = 0; i < ticket; ++i) {
			printf("ticket [%d]\n", i + 1); //몇번째 티켓인지 
			for (j = 0; j < 6; ++j) {
				printf("%d ", arr[bu - ticket + i][j]); // 뽑은 번호를 출력해준다

				for (k = 0; k < 6; k++) {
					if (arr[bu - ticket + i][j] == dangchum[k]) {
						count++;// 보너스번호가 아닌 일반 번호와 몇개 일치하는지
					}
				}
				if (arr[bu - ticket + i][j] == dangchum[6]) count2++;//보너스 번호랑 일치하는지

			}
			if (soonwee2(count, count2) != 0) {
				printf("ㅊㅋㅊㅋ %d등!!!!", soonwee2(count, count2));// 당첨되면 알려준다
				soonwee[soonwee2(count, count2) - 1]++;//몇등이 몇번당첨 됬는지 세주기
			}
			printf("\n\n");

			count = 0;
			count2 = 0;
		}
	}

	return yn;//메뉴4에서 당첨될때까지 받기위해 필요한 수 리턴
}
