#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 4
#define NBUFF 10

sem_t sem,sem_produce,sem_consume;
int var[NBUFF];
int iProduce=0, iConsume=0;
int nProdotti=0, nConsumati=0;

void *produce(void *arg);
void *consume(void *arg);

int main(int argc, char const *argv[])
{
	pthread_t tid[N];
	pthread_t tidConsume[N];
	int i;

	sem_init(&sem,0,1);
	sem_init(&sem_produce,0,NBUFF);
	sem_init(&sem_consume,0,0);


	for(i=0;i<N;i++)
		pthread_create(&tid[i],NULL,produce,NULL);
	for(i=0;i<N;i++)
		pthread_create(&tidConsume[i],NULL,consume,NULL);

	for(i=0;i<N;i++){
		pthread_join(tid[i],NULL);
		pthread_join(tidConsume[i],NULL );
	}

	sem_destroy(&sem);
	sem_destroy(&sem_produce);
	sem_destroy(&sem_consume);

	return 0;
}

void *produce(void *arg){
	int i;
	while(nProdotti <= 30){
		sem_wait(&sem_produce);
		sem_wait(&sem);
		var[iProduce%NBUFF] = iProduce % NBUFF;
		printf("Prodotto in pos %d\n",iProduce % NBUFF);
		iProduce++;
		nProdotti++;
		sem_post(&sem);
		sem_post(&sem_consume);
		sleep(1);
		
	}
	return NULL;
}

void *consume(void *arg){
	int i;
	while(nConsumati<=30){
		sem_wait(&sem_consume);
		sem_wait(&sem);
		printf("Il valore della posizione %d è %d\n",iConsume % NBUFF,var[iConsume%NBUFF]);
		iConsume++;
		nConsumati++;
		sem_post(&sem);
		sem_post(&sem_produce);
		sleep(2);
		
	}
	return NULL;
}