
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define COUNTING_NUMBER 50
void writer();
void reader();

sem_t writer_lock;
sem_t reader_count_lock;

long cur_writer=0;
int cur_count = 0;
int reader_count = 0;


int main() {
	int i;
	//�������� �ʱ�ȭ
	sem_init(&writer_lock, 0, 1);
	sem_init(&reader_count_lock, 0, 1);

	//2���� reader�� 5���� writer ����
	pthread_t thread_writer_0, thread_writer_1, thread_writer_2, thread_writer_3, thread_writer_4;
	pthread_t thread_reader_0, thread_reader_1;

	//������ ����
	pthread_create(&thread_writer_0, NULL, (void*)&writer, NULL);
	pthread_create(&thread_writer_1, NULL, (void*)&writer, NULL);
	pthread_create(&thread_writer_2, NULL, (void*)&writer, NULL);
	pthread_create(&thread_writer_3, NULL, (void*)&writer, NULL);
	pthread_create(&thread_writer_4, NULL, (void*)&writer, NULL);

	pthread_create(&thread_reader_0, NULL, (void*)&reader, NULL);
	pthread_create(&thread_reader_1, NULL, (void*)&reader, NULL);

	//�����尡 ����� �� ���� ��ٸ���
	pthread_join(thread_writer_0, NULL);
	pthread_join(thread_writer_1, NULL);
	pthread_join(thread_writer_2, NULL);
	pthread_join(thread_writer_3, NULL);
	pthread_join(thread_writer_4, NULL);

	pthread_join(thread_reader_0, NULL);
	pthread_join(thread_reader_1, NULL);

	//�������� �ı�
	sem_destroy(&writer_lock);
	sem_destroy(&reader_count_lock);

}



void writer()
{
	int now_count = 0;
	for (int i = 0; i < COUNTING_NUMBER; i++)
	{
		usleep(100000);
		//lock
		sem_wait(&writer_lock);

		//critical
		now_count++;
		cur_writer = pthread_self();
		cur_count = now_count;
		//printf("<Writer> writer : %ld - total count: %d\n",cur_writer,cur_count);

		//unlock
		sem_post(&writer_lock);
		
	}
		

	
}

void reader()
{

	for (int i = 0; i < COUNTING_NUMBER; i++)
	{
		usleep(30000);
		//lock
		sem_wait(&reader_count_lock);
		//reader�� �� ��ŭ ����
		reader_count++;
		//ó�� ���� reader��� writer_lock�� ȹ���Ѵ�
		if (reader_count == 1) sem_wait(&writer_lock);
		sem_post(&reader_count_lock);

		//critical
		printf("<Reader>: %ld => <Recent writer>: %ld - total count : %d\n", pthread_self(),cur_writer, cur_count);
		

		//unlcok
		sem_wait(&reader_count_lock);
		//critical�� �����ִ� reader ������ �ϳ� ���δ�
		reader_count--;
		//���� �ڽ��� critical �ȿ� �����ϴ� ������ reader��� writer lock�� �����ش�.
		if (reader_count == 0) sem_post(&writer_lock);
		sem_post(&reader_count_lock);
		
	}
}
