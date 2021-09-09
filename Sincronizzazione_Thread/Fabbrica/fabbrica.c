#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define N 10 
#define M 5
#define K 3

sem_t infornatori,forno,controllore,controlla,indice_infornatore,indice_controllore;
int piastrelle[M];
int i_controllore=0,i_infornatore=0;

void *infornare(void *arg);
void *controllare(void *arg);

int main(int argc, char const *argv[])
{
	pthread_t tidInfornatore[N+5],tidControllore[M+5];
	int i;
	srand(time(NULL));
	sem_init(&infornatori,0,N);
	sem_init(&forno,0,M);
	sem_init(&controlla,0,0);
	sem_init(&controllore,0,K);
	sem_init(&indice_controllore,0,1);
	sem_init(&indice_infornatore,0,1);

	for(i=0;i<M;i++)
		piastrelle[i]=-1;

	for(i=0;i<M+5;i++)
		pthread_create(&tidControllore[i],NULL,controllare,NULL);

	for(i=0;i<N+5;i++)
		pthread_create(&tidInfornatore[i],NULL,infornare,NULL);

	for(i=0;i<M+5;i++)
		pthread_join(tidControllore[i],NULL);

	for(i=0;i<N+5;i++)
		pthread_join(tidInfornatore[i],NULL);

	sem_destroy(&infornatori);
	sem_destroy(&forno);
	sem_destroy(&controlla);
	sem_destroy(&indice_controllore);
	sem_destroy(&indice_infornatore);




	return 0;
}

void *infornare(void *arg){
	sleep(2);
	while(1){
		sleep(2);
		sem_wait(&infornatori);
		sem_wait(&forno);
		sem_wait(&indice_infornatore);
		piastrelle[i_infornatore] = rand()%2;
		printf("INFORNATO %d valore %d\n",i_infornatore,piastrelle[i_infornatore]);
		i_infornatore++;
		i_infornatore=i_infornatore%M;
		sleep(2+rand()%4);
		sem_post(&controlla);
		sem_post(&indice_infornatore);
		sem_post(&infornatori);
	}

}


void *controllare(void *arg){
	sleep(2);
	while(1){
		sleep(1);
		sem_wait(&controllore);
		sem_wait(&indice_controllore);
		sem_wait(&controlla);

		sleep(1+rand()%4);
		if(piastrelle[i_controllore] == 1){
			printf("PIASTRELLA 	%d OK\n",i_controllore);
		}
		else {
			printf("PIASTRELLA %d DIFETTATA\n",i_controllore);
			sem_post(&forno);
			sem_wait(&controlla);
			i_controllore = (i_controllore+1)%M;
			sleep(1+rand()%4);
			if(piastrelle[i_controllore] == 1)
				printf("SECONDA PIASTRELLA %d OK\n",i_controllore);
			else
				printf("SECONDA PIASTRELLA %d DIFETTATA\n",i_controllore);
		}
		i_controllore = (i_controllore+1)%M;
		sem_post(&forno);
		sem_post(&indice_controllore);
		sem_post(&controllore);
	}

}