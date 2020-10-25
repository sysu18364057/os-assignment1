#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<time.h>
#include<math.h>
#include<pthread.h>
#define NUM_THREADS 3
#define BUFFER_SIZE 20

typedef struct buf
{
	int rear;
	int front;
	int buffer[20];
}buffer;

int exp_random(double lambda)
{
	double pv = 0.0;
	while(1)
	{
		pv = (double)rand()/(double)RAND_MAX;
		if(pv ！= 1)
		{
			break;
		}
	}
	pv = (-1.0/lambda)*log(1-pv);
	return (int)(pv*1000000);
	
}

sem_t *full;
sem_t *empty;
void *ptr;
sem_t *s_mutex;

void *consumer(void *param)
{
	int lambda = *(int*) param;
	int random_time = exp_random(lambda);
	pthread_t tid = pthread_self();
	do{
		int random_time = exp_random(lambda);
		usleep(random_time);
		struct buf *shm_ptr = ((struct buf*)ptr);
		sem_wait(full);
		sem_wait(s_mutex);
		int item = shm_ptr->buffer[shm_ptr->front];
		shm_ptr->front = (shm_ptr->front+1)%BUFFER_SIZE;
		sem_post(s_mutex);
		sem_post(empty);
		printf("consuming the data %d by pid %lu\n",item,(unsigned long)tid);
	}while(1);
	pthread_exit(0);
}

int main(int argc,char** argv)
{
	full = sem_open("full",O_CREAT);
	empty = sem_open("empty",O_CREAT);
	s_mutex = sem_open("mutex",O_CREAT);
	int shm_fd = shm_open("buffer",O_RDWR,0666);
	ptr = mmap(0,sizeof(struct buf),PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,0);
	pthread_t tid1,tid2,tid3;
	pthread_attr_t attr1,attr2,attr3;
	int lambda = atoi(argv[1]);
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_init(&attr3);
	pthread_create(&tid1,&attr1,consumer,&lambda);
	pthread_create(&tid2,&attr2,consumer,&lambda);
	pthread_create(&tid3,&attr3,consumer,&lambda);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	munmap(ptr,sizeof(struct buf));
	shm_unlink("buffer");
	sem_close(full);
	sem_close(empty);

	return 0;
}

