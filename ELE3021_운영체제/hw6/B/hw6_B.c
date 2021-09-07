
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define COUNTING_NUMBER 50
void chopstick();
sem_t grab[6];
sem_t cnt_lock;
int cnt = 0;


int main() {
	int i;
	//�������� �ʱ�ȭ
	sem_init(&grab[0], 0, 1);
	sem_init(&grab[1], 0, 1);
	sem_init(&grab[2], 0, 1);
	sem_init(&grab[3], 0, 1);
	sem_init(&grab[4], 0, 1);
	sem_init(&grab[5], 0, 1);

	sem_init(&cnt_lock, 0, 1);
	

	//5���� ö���� ������ ����
	pthread_t philoso_0, philoso_1, philoso_2, philoso_3, philoso_4, philoso_5;


	//������ ����
	pthread_create(&philoso_0, NULL, (void*)&chopstick, NULL);
printf("0 philoso create\n");
	pthread_create(&philoso_1, NULL, (void*)&chopstick, NULL);
printf("1 philoso create\n");
	pthread_create(&philoso_2, NULL, (void*)&chopstick, NULL);
printf("2 philoso create\n");
	pthread_create(&philoso_3, NULL, (void*)&chopstick, NULL);
printf("3 philoso create\n");
	pthread_create(&philoso_4, NULL, (void*)&chopstick, NULL);
printf("4 philoso create\n");
	pthread_create(&philoso_5, NULL, (void*)&chopstick, NULL);
printf("5 philoso create\n");


	

	//�����尡 ����� �� ���� ��ٸ���
	pthread_join(philoso_0, NULL);
	pthread_join(philoso_1, NULL);
	pthread_join(philoso_2, NULL);
	pthread_join(philoso_3, NULL);
	pthread_join(philoso_4, NULL);
	pthread_join(philoso_5, NULL);

	printf("All philosophers have finished eating\n.");

	//�������� �ı�
	sem_destroy(&grab[0]);
	sem_destroy(&grab[1]);
	sem_destroy(&grab[2]);
	sem_destroy(&grab[3]);
	sem_destroy(&grab[4]);
	sem_destroy(&grab[5]);

	sem_destroy(&cnt_lock);
	

}



void chopstick()
{
	

	//�ڽ��� ���° �ڸ��� ���� ö�������� ã��
	sem_wait(&cnt_lock);
	int order = cnt;
	printf("my order is %d\n", order);
	cnt++;
	sem_post(&cnt_lock);


	
	//�ڽ��� Ȧ�� ��°�� �ڸ� ö���ڶ�� ���� ������ ���� ��´�
	if (order % 2 == 1) {

		//���� lock
		sem_wait(&grab[(order+1)%6]);
printf("\t%d - philoso success grab left\n", order);

		//������lock
		sem_wait(&grab[order]);
printf("\t%d - philoso success grab right\n", order);
	}
	//�ڽ��� ¦����° �ڸ� ö���ڶ�� ������ ������ ���� ��´�
	else {
		//������ lock

		sem_wait(&grab[order]);
printf("\t%d - philoso success grab right\n", order);

		//����lock
		sem_wait(&grab[(order+1)%6]);
printf("\t%d - philoso success grab left\n", order);
	}
	



	//�� ������ ��� ��µ� �����ߴٸ� �Ļ縦 �Ѵ�
	//eating....(critical section)
	usleep(10000);
	printf("\t%d - philoso eating...\n\n", order);


	//�Ļ縦 ���ƴٸ� �� �������� ���� ���´�
	sem_post(&grab[order]);
printf("\t%d - philoso success release one\n", order);
	sem_post(&grab[(order+1)%6]);
printf("\t%d - philoso success release two\n", order);
	printf("%d - philoso finish eating\n\n", order);


}


