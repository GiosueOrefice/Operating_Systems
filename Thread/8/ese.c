#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define N_PASSEGGERI 30

#define N 3
#define M 3

sem_t accesso,porte[3];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int numPersoneScendono[3];
int cont=0;
void *passeggeri(void *);

int main(int argc, char const *argv[])
{
	
	pthread_t tid[N_PASSEGGERI];
	int i;

	sem_init(&accesso,0,N+M);
	sem_init(&porte[0],0,1);
	sem_init(&porte[1],0,1);
	sem_init(&porte[2],0,1);

	srand(time(NULL));
	for(i=0;i<3;i++)
		numPersoneScendono[i]=0;

	for(i=0;i<N_PASSEGGERI;i++)
		pthread_create(&tid[i],NULL,passeggeri,NULL);

	for(i=0;i<N_PASSEGGERI;i++)
		pthread_join(tid[i],NULL);

	sem_destroy(&accesso);
	for(i=0;i<3;i++)
		sem_destroy(&porte[i]);

	return 0;
}

void *passeggeri(void *arg){
	int val;
	int entrata =  rand()%3;
	int uscita;
	sem_wait(&accesso);
	sem_wait(&porte[entrata]);
	while(numPersoneScendono[entrata]>0){
		printf("sto aspettando per la porta %d %lu\n",entrata,pthread_self());
		sem_post(&porte[entrata]);
		sleep(1);
		sem_wait(&porte[entrata]);
		
		
	};
	
	printf("ENTRATO %lu DALLA PORTA %d\n",pthread_self(),entrata);

	sem_post(&porte[entrata]);
	sleep(1+rand()%2);
	uscita = rand()%3;


	pthread_mutex_lock(&mutex);
	numPersoneScendono[uscita]++;
	pthread_mutex_unlock(&mutex);
	sem_wait(&porte[uscita]);
	sleep(4);
	printf("USCITO %lu DALLA PORTA %d\n",pthread_self(),uscita);
	pthread_mutex_lock(&mutex);
	numPersoneScendono[uscita]--;
	pthread_mutex_unlock(&mutex);
	sem_post(&porte[uscita]);
	sem_post(&accesso);

	return NULL;
}