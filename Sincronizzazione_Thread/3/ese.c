#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

void *scrittore(void *arg);
void *lettore(void *arg);
void *stampa(void *arg);

#define N 10
struct info
{
	
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int cont;
	sem_t *sem1,*sem2;
	int fd;
} data = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,0};



int main(int argc, char const *argv[])
{
	pthread_t tScrittore,tLettore,tStampa;
	int i;


	sem_unlink("/sem1");
	sem_unlink("/sem2");

	data.sem1 = sem_open("/sem1",O_CREAT,0777,1);
	data.sem2 = sem_open("/sem2",O_CREAT,0777,0);

	data.fd = open("numeri.txt",O_RDWR | O_CREAT | O_TRUNC,0666);

	pthread_create(&tScrittore,NULL,scrittore,NULL);
	pthread_create(&tLettore,NULL,lettore,NULL);
	pthread_create(&tStampa,NULL,stampa,NULL);

	pthread_join(tScrittore,NULL);
	pthread_join(tLettore,NULL);
	pthread_join(tStampa,NULL);

	close(data.fd);
	sem_unlink("/sem1");
	sem_unlink("/sem2");

	return 0;
}

void *scrittore(void *arg){
	struct timespec time;
	time.tv_sec = 0;
	time.tv_nsec = 500000000L;
	int a=0, b=1;
	int f;
	int i;
	for(i=0;i<N;i++){
		sem_wait(data.sem1);
		f=a+b;
		a=b;
		b=f;
		lseek(data.fd,0,SEEK_SET);
		write(data.fd,&b,sizeof(int));
		printf("Ho scritto %d\n",b);
		sem_post(data.sem2);
		nanosleep(&time,NULL);

	}
	return NULL;
}

void *lettore(void *arg){
	int num;
	while(1){
		sem_wait(data.sem2);
		lseek(data.fd,0,SEEK_SET);
		read(data.fd,&num,sizeof(int));
		printf("Ho letto il numero %d\n",num);
		pthread_mutex_lock(&data.mutex);
		data.cont++;
		if(data.cont  == N){
			pthread_cond_signal(&data.cond);
			pthread_mutex_unlock(&data.mutex);
			return NULL;
		}
		pthread_mutex_unlock(&data.mutex);
		sem_post(data.sem1);
	}
}

void *stampa(void *arg){
	pthread_mutex_lock(&data.mutex);
	pthread_cond_wait(&data.cond,&data.mutex);
	printf("OPERAZIONI CONCLUSE, ARRIVEDERCI DAL THREAD %lu\n",pthread_self());
	sleep(5);
	return NULL;
}
