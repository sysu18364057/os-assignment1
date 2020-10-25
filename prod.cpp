#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<math.h>
#include<time.h>
#include<pthread.h>

#define NUM_THREADS 3
#define MAX_PRODUCT 20
#define BUFFER_SIZE 20

typedef struct buf
{
	int rear;
	int front;
	int buffer[20];
};

int exp_random(double lambda)
{
	double pv = 0.0;
	while(1)
	{
		pv = (double)rand()/(double)RAND_MAX;
		if(pv != 1)
		{
			break;
		}
	}
	pv = (-1.0/lambda)*log(1-pv);
	return (int)(pv*1000000);
}

sem_t *full;
sem_t *empty;
sem_t *s_mutex;
pthread_t tid[NUM_THREADS];
void *ptr;


void *producer(void *param)
{
	double lambda = (*(int*)param) * 1.0;
	pthread_t tid = pthread_self();

	do{
		int sleep_time = exp_random(lambda);
		usleep(sleep_time);
		int item = rand() % 10;
		struct buf *shm_ptr = ((struct buf*)ptr);
		sem_wait(empty);
		sem_wait(s_mutex);
		printf("producing the data %d to buffer[%d] by id %lu\n",item,shm_ptr->rear,(unsigned long)tid);
		shm_ptr->buffer[shm_ptr->rear] = item;
		shm_ptr->rear = (shm_ptr->rear+1)%BUFFER_SIZE;
		sem_post(s_mutex);
		sem_post(full);
	}while(1);
	pthread_exit(0);
}

int main (int argc,char** argv)
{
	struct buf shareMemory;
	memset(&shareMemory,0,sizeof(struct buf));
	//初始化内存
	full = sem_open("full",O_CREAT,0666,0);
	empty = sem_open("empty",O_CREAT,0666,0);
	s_mutex = sem_open("mutex",O_CREAT,0666,0);
	//O_CREAT表示信号若存在则什么也不做也不报错
	
	sem_init(full,1,0);
	sem_init(empty,1,BUFFER_SIZE);
	sem_init(s_mutex,1,1);
	//指向信号的指针，为0时此信号共享，否则不共享，信号量初始值
	
	int shm_fd = shm_open("buffer",O_CREAT|O_RDWR,0666);
	ftruncate(shm_fd,sizeof(struct buf));
	ptr = mmap(0,sizeof(struct buf),PROT_WRITE,MAP_SHARED,shm_fd,0);

	int lambda = atoi(argv[1]);
	int lambda_1 = 1;
	int lambda_2 = 2;
	int lambda_3 = 3;
	pthread_t tid1,tid2,tid3;
	pthread_attr_t attr1,attr2,attr3;
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_init(&attr3);
	pthread_create(&tid1,&attr1,producer,&lambda);
	pthread_create(&tid2,&attr2,producer,&lambda);
	pthread_create(&tid3,&attr3,producer,&lambda);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	return 0;
}
