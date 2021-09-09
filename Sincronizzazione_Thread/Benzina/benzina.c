#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>


#define N 3
#define M 30

int litriRimanenti = M;
int numAuto=0;

sem_t sem_litriRimanenti;
sem_t sem_numAuto;
sem_t pompe;

void *automobilista(void *arg);
void *autobotte(void *arg);

int main(int argc, char const *argv[])
{
	pthread_t tid[10];
	pthread_t threadAutobotte;
	int i;
	srand(time(NULL));

	sem_init(&sem_numAuto,0,1);
	sem_init(&sem_litriRimanenti,0,1);
	sem_init(&pompe,0,N);


	pthread_create(&threadAutobotte,NULL,autobotte,NULL);

	for(i=0;i<10;i++){
		pthread_create(&tid[i],NULL,automobilista,NULL);
		sleep(2);
	}

	for(i=0;i<10;i++)
		pthread_join(tid[i],NULL);

	sem_destroy(&sem_litriRimanenti);
	sem_destroy(&sem_numAuto);
	sem_destroy(&pompe);

	return 0;
}

void *automobilista(void *arg){

	sleep(1+rand()%3);
	int numLitriRicarica = 1 + rand()%15;
	sem_wait(&pompe);
	sem_wait(&sem_litriRimanenti);
	if(litriRimanenti>=numLitriRicarica){
		sem_wait(&sem_numAuto);
		numAuto++;
		sem_post(&sem_numAuto);
		litriRimanenti-=numLitriRicarica;
		sleep(1+rand()%2);
		printf("FATTO BENZINA di %d litri - litriRimanenti %d\n",numLitriRicarica,litriRimanenti);
		sem_wait(&sem_numAuto);
		numAuto--;
		sem_post(&sem_numAuto);
	}

	else{
		printf("LITRI NON NECESSARI %d %d\n",numLitriRicarica,litriRimanenti);
		
	}

	sem_post(&sem_litriRimanenti);
	sem_post(&pompe);

	return NULL;
	
}

void *autobotte(void *arg){
	while(1){
		sleep(4);

		sem_wait(&sem_litriRimanenti);
		sem_wait(&sem_numAuto);	
		if(numAuto == 0 && litriRimanenti<M){
			litriRimanenti = M;
			sleep(1+rand()%3);
			printf("AUTOBOTTE RICARICA COMPLETATA\n");
		}

		else 
			printf("CI SONO  %d AUTO A FARE BENZINA NON POSSO RICARICARE L'AUTOBOTTE\n",numAuto);

		sem_post(&sem_numAuto);
		sem_post(&sem_litriRimanenti);
	}

	return NULL;

}