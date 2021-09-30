#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void printMenu();//�޴��� ����Ʈ�ϴ� �Լ�
void print(int * arr, int size);//�迭�� ����Ʈ���ִ� �Լ�
int * newTicket();//Ƽ�� �̾��ִ� �Լ�
int* dangchum();//��÷ ���� ���� ���� �Լ�
int soonwee2(int count, int count2);//���� �������ִ� �Լ�
int soonwee1(int* soonwee, int* dangchum, int** arr, int ticket, int cnt, int bu);//������ Ƽ�� ��ȣ�� �����ְ�, ��÷�Ȱ�� ������� �������ָ�, ����� �����÷����� �����ִ� �Լ�


int main() {
	int menu, ticket;
	int totalpr = 0, totalbu = 0, earn, count = 0; // �� �ݾ�, �� Ƽ�ϱ��ż�, ����, ���° ��������
	char yn; //�޴�4���� y or n �Է¹��� ����
	int pr1 = 2000000000, pr2 = 100000000, pr3 = 2000000, pr4 = 50000, pr5 = 5000; // ��÷ �ݾ�
	int i, j;
	int **ticarr = NULL; //��ü Ƽ�� ����� �־��� ������ �迭
	int soonwee[5] = { 0,0,0,0,0 };//�� ���� �� �� ��÷����� �־��� �迭
	int *dang = dangchum(); //��÷ ���ڸ� ���� �迭

	srand(time(NULL));

	while (1) {

		//�޴� ����Ʈ�ϰ� �Է¹ޱ�
		printMenu();
		scanf("%d", &menu);

		count++;//�� ��° �������� ���ֱ�

		//�޴�1 ����
		if (menu == 1) {

			//��� Ƽ���� ���� �Է¹ޱ�
			printf("How many lines will you buy? : ");
			scanf("%d", &ticket);
			printf("\n");

			totalpr += 1000 * ticket;//�� ���� ���� �����ֱ�
			totalbu += ticket;//�� ����Ƽ�� ���� �����ֱ�

			//ó�� �����϶�
			if (count == 1) {

				//������ �迭 ���� �Ҵ�
				ticarr = (int **)malloc(ticket * sizeof(int *));


				for (i = 0; i < ticket; i++) { // ������ ������ŭ Ƽ���� �̾��ش�
					ticarr[i] = newTicket();
				}

				soonwee1(soonwee, dang, ticarr, ticket, count, totalbu);
			}

			//�ι��� �̻��� �����϶�
			else {
				ticarr = (int**)realloc(ticarr, totalbu * sizeof(int*));// �� Ƽ�� ���� ������ŭ �Ҵ� �߰��� �����ֱ�

				for (i = 0; i < ticket; i++) { // ������ ������ŭ Ƽ���� �̾��ش�
					ticarr[totalbu - ticket + i] = newTicket();
				}

				soonwee1(soonwee, dang, ticarr, ticket, count, totalbu);
			}
		}

		//�޴�2����
		else if (menu == 2) {
			printf("You bought %d tickets, and the total cost is %d won\n\n", totalbu, totalpr); // �� Ƽ�ϰ����� �� �ݾ����
		}

		//�޴�3 ����
		else if (menu == 3) {
			earn = soonwee[0] * pr1 + soonwee[1] * pr2 + soonwee[2] * pr3 + soonwee[3] * pr4 + soonwee[4] * pr5;//�� �� ���
			printf("1st : %d 2nd : %d 3rd : %d 4th : %d 5th : %d\n", soonwee[0], soonwee[1], soonwee[2], soonwee[3], soonwee[4]);
			printf("You earned %d won\n\n", earn); // �� �� ���
		}

		//�޴�4 ����
		else if (menu == 4) {
			int menu4yn;//soonwee1�Լ����� ���ϵ� yn���� ������ ���� ����
			int menu4pr, menu4ea;//�޴�4���� �� ���ұݾ�,��÷�ݾ�
			int t;//�޴�4������ Ƽ�� ������ �� �� ���

			//y or n �Է� �ޱ�
			printf("Are you sure? (y or n) : ");
			scanf("%s", &yn);

			//y�� �Է����� ��
			if (yn == 'y') {
				//ó�� �����϶�
				if (count == 1) {

					//������ �迭 ���� �Ҵ�
					ticarr = (int **)malloc(sizeof(int *));

					menu4pr = 0;
					menu4ea = 0;
					t = 0;
					while (1) {
						
						int *ticarr2[1];//������ ���� �־��� �迭 ����

						ticarr[t] = newTicket(); // ��ü Ƽ�� ��Ͽ� �߰����ֱ�
						ticarr2[0] = ticarr[t]; // �ϳ��� ���� soonwee1�Լ��� �־��־�� �ϹǷ� ���� �����صд�
						t++;
						ticarr = (int **)realloc(ticarr, (t + 1) * sizeof(int *));//������ �迭�� ��ĭ�� while���� �������� �÷��ش�
						totalbu++; // �� Ƽ�� ���ż� �߰����ֱ�
						totalpr += 1000; // �� ���� ���ݿ� �߰����ֱ�

						//Ƽ�ϼ��� ���徿 �����ϹǷ� �Ű�����'ticket'�ڸ��� 1�� �־��ش�
						menu4yn = soonwee1(soonwee, dang, ticarr2, 1, count, totalbu);
						menu4pr = 1000 * t;//�޴�4���� ������ ������ ������ش�

						//�޾ƿ� yn�� ���� ���� ��÷�ݾ��� �������ش�
						if (menu4yn == 1) menu4ea = pr1;//1��
						if (menu4yn == 2) menu4ea = pr2;//2��
						if (menu4yn == 3) menu4ea = pr3;//3��
						if (menu4yn == 4) menu4ea = pr4;//4��


						if (menu4yn == 1||menu4yn==2) break; //soonwee1���� ���ϵ� yn�� ���� 1�Ǵ� 2�Ǵ� 3�Ǵ� 4�� ��� 4���̻��� ��÷�� ���̹Ƿ� while�� Ż��

					}
					printf("total price for menu4 : %d\nyou earned %d\n", menu4pr, menu4ea);
				}

				//�ι��� �̻��� �����϶�
				else {
					menu4pr = 0;
					menu4ea = 0;
					t = 0;
					while (1) {
						
						int *ticarr2[1];//������ ���� �־��� �迭 ����

						ticarr = (int**)realloc(ticarr, (totalbu + 1) * sizeof(int*));// while���� ���� ���� �ϳ��� �� �߰��Ҵ����ش�

						ticarr[totalbu] = newTicket();// ��ü Ƽ�� ��Ͽ� �߰����ֱ�
						ticarr2[0] = ticarr[totalbu];// �ϳ��� ���� soonwee1�Լ��� �־��־�� �ϹǷ� ���� �����صд�
						totalbu++; // �� Ƽ�� ���ż� �߰�
						totalpr += 1000;//�� ���� ���ݿ� �߰�
						t+=1;

						//Ƽ�ϼ��� ���徿 �����ϹǷ� �Ű����� ticket�ڸ��� 1�� �־��ְ�, 
						//��ü Ƽ�� ����� �ƴ϶� �ϳ����� Ƽ�� ��ϸ� �Ѱ��ֹǷ� count���� 1�� �Ѱ��־�  soonwee1�Լ����� �ε��������� �����ʵ����Ѵ�
						menu4yn = soonwee1(soonwee, dang, ticarr2, 1, 1, totalbu);
						menu4pr = 1000 * t;//�޴�4���� ���ҵ� �ݾ� ���

						//�޾ƿ� yn�� ���� ���� ��÷�� �ݾ��� �������ش�
						if (menu4yn == 1) menu4ea = pr1;//1��
						if (menu4yn == 2) menu4ea = pr2;//2��
						if (menu4yn == 3) menu4ea = pr3;//3��
						if (menu4yn == 4) menu4ea = pr4;//4��

						if (menu4yn == 1 || menu4yn == 2) break; //soonwee1���� ���ϵ� yn�� ���� 1�Ǵ� 2�Ǵ� 3�Ǵ� 4�� ��� 4�� �̻��� ��÷�� ���̹Ƿ� while�� Ż��

					}
					printf("total price for menu4 : %d\nyou earned %d\n", menu4pr, menu4ea);
				}
			}

			//y �̿��� ���� �Է����� ��
			else {
				printf("\n");
				count--; // ���� ���� ����ȳ�ġ�� �Ѵ�
				continue;
			}
		}

		//�޴�0 ����
		else if (menu == 0) break;

		//�̿��� ���ڸ� �Է� ���� �� �ٽ� �Է� �ޱ�
		else {
			printf("Error : input again\n");
			count--; // ���� ���� ������ ��ġ�� �ʵ��� �Ѵ�
			continue;
		}
	}


	//�޸𸮸� ���������ش�
	for (i = 0; i < totalbu; ++i) {
		free(ticarr[i]);
	}
	free(ticarr);
}





///////////////////////////////////////////////////////////////////////////////




//�޴��� ����Ʈ�ϴ� �Լ�
void printMenu() {
	printf("------------------Lottery Ticket------------------\n");
	printf("--------------------------------------------------\n1. Buy tickets\n2. Spent money\n3. Earned money\n4. Buy until I get 4th of higher prize\n0. exit\n");

}





////////////////////////////////////////////////////////////////////////////////





//�迭�� ����Ʈ���ִ� �Լ�
void print(int * arr, int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("%d ", arr[i]);

	}
	printf("\n\n");
}





/////////////////////////////////////////////////////////////////////////////////





//Ƽ�ϻ̾��ִ� �Լ�
int * newTicket() {
	int i, j, temp, count = 0;
	int *ticketarr = NULL;

	ticketarr = (int*)malloc(6 * sizeof(int));

	//�������� �̾��ֱ�
	for (i = 0; i < 6; ++i) {
		while (1) {
			ticketarr[i] = rand() % 45 + 1;
			for (j = 0; j < i; ++j) {//�ߺ����� �Ȼ����� ����
				if (ticketarr[i] == ticketarr[j]) count++;
			}
			if (count == 0) break;
			count = 0;

		}
	}

	//ũ������� ����
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






//��÷���� ���س��� �Լ�
int* dangchum() {
	int i, bonus, j, temp, count = 0;
	int *dangchum = NULL;

	dangchum = (int*)malloc(7 * sizeof(int));

	//�������� �̾��ֱ�
	for (i = 0; i < 6; ++i) {
		while (1) {
			dangchum[i] = rand() % 45 + 1;
			for (j = 0; j < i; ++j) { // �ߺ����� �Ȼ����� ����
				if (dangchum[j] == dangchum[i]) count++;
			}
			if (count == 0) break;
			count = 0;

		}
	}

	//���ʽ� ��ȣ�ϳ� �̰� ������ ĭ�� ����
	while (1) {
		count = 0;
		bonus = rand() % 45 + 1;
		for (j = 0; j < 6; ++j) { // �ߺ����� �Ȼ����� ����
			if (dangchum[j] == bonus) count++;
		}
		if (count == 0) break;
	}
	dangchum[6] = bonus;


	//ũ������� ����- ������ ���ʽ���ȣ�� ��������
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






//�����������ִ� �Լ�
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






//������ Ƽ�� ��ȣ�� �����ְ�, ��÷�� ��� ������� �������ָ�, ����� �����÷����� �����ִ� �Լ�
int soonwee1(int* soonwee, int* dangchum, int** arr, int ticket, int cnt, int bu) {
	int i, j, k;
	int count = 0, count2 = 0;//�Ϲݹ�ȣ�� � ��ġ�ϴ��� ���� ����, ���ʽ���ȣ�� ��ġ�ϴ��� ���� ����
	int yn = 0; //�޴�4���� ��÷�ɶ����� �ޱ����� �ʿ��� �� ����



	//ó�� ����������
	if (cnt == 1) {

		for (i = 0; i < ticket; ++i) {
			printf("ticket [%d]\n", i + 1);//���° Ƽ������ ����Ʈ
			for (j = 0; j < 6; ++j) {
				printf("%d ", arr[i][j]); // ���� ��ȣ�� ���
				for (k = 0; k < 6; k++) {
					if (arr[i][j] == dangchum[k]) {
						count++;//���ʽ���ȣ�� �ƴ� �Ϲݹ�ȣ�� � ��ġ�ϴ���
					}
				}
				if (arr[i][j] == dangchum[6]) count2++;//���ʽ� ��ȣ�� ��ġ�ϴ���

			}
			if (soonwee2(count, count2) != 0) {
				if (soonwee2(count, count2) ==1) yn=1;// �޴�4���� ��÷�ɶ����� �ޱ����� �ʿ��� �� �����
				if (soonwee2(count, count2) == 2) yn = 2;
				if (soonwee2(count, count2) == 3) yn = 3;
				if (soonwee2(count, count2) == 4) yn = 4;
				if (soonwee2(count, count2) == 5) yn = 5;
				printf("�������� %d��!!!!", soonwee2(count, count2));//��÷�Ǹ� �˷��ش�
				soonwee[soonwee2(count, count2) - 1]++; // ����� �����÷����� ���ֱ�
			}
			printf("\n\n");

			count = 0;
			count2 = 0;
		}
	}




	//�ι�° �̻��� �����϶�
	else {
		for (i = 0; i < ticket; ++i) {
			printf("ticket [%d]\n", i + 1); //���° Ƽ������ 
			for (j = 0; j < 6; ++j) {
				printf("%d ", arr[bu - ticket + i][j]); // ���� ��ȣ�� ������ش�

				for (k = 0; k < 6; k++) {
					if (arr[bu - ticket + i][j] == dangchum[k]) {
						count++;// ���ʽ���ȣ�� �ƴ� �Ϲ� ��ȣ�� � ��ġ�ϴ���
					}
				}
				if (arr[bu - ticket + i][j] == dangchum[6]) count2++;//���ʽ� ��ȣ�� ��ġ�ϴ���

			}
			if (soonwee2(count, count2) != 0) {
				printf("�������� %d��!!!!", soonwee2(count, count2));// ��÷�Ǹ� �˷��ش�
				soonwee[soonwee2(count, count2) - 1]++;//����� �����÷ ����� ���ֱ�
			}
			printf("\n\n");

			count = 0;
			count2 = 0;
		}
	}

	return yn;//�޴�4���� ��÷�ɶ����� �ޱ����� �ʿ��� �� ����
}
