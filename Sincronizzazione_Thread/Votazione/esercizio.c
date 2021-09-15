#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

/*
	2 insiemi di processi che a turno votano 
	al turno del primo insieme di processi, ogni processo vota uno dei processi del secondo insieme.
 	una volta che hanno votato tutti:
	l'ultimo processo che vota produce e calcola automaticamente il processo del secondo cluster che ha vinto e 
	lo sveglia,quando si sveglia questo sveglia tutti gli altri processi del secondo insieme che 
	devono votare per il primo insieme.
	all'infinito
*/

#define N_PROCESSI_UNO 5
#define N_PROCESSI_DUE 4

int voti_uno[N_PROCESSI_DUE]; //array utilizzato per salvare i voti  del primo gruppo 
int voti_due[N_PROCESSI_UNO]; //array per i voti del secondo gruppo

int num_votanti_uno = N_PROCESSI_UNO; //int che indica quanti thread devono ancora votare
int num_votanti_due = N_PROCESSI_DUE;
sem_t sem_num_votanti1,sem_num_votanti2; //per la mutua esclusione delle variabili di sopra

int winner1,winner2; //per indiciare i vincitori

pthread_mutex_t mutex1,mutex2; 
pthread_cond_t cond1,cond2; //per avvisare i processi dell'altro gruppo che e' terminata la votazione

void init();
void initVoti(int *,int);
int findMax(int *,int);

void *processi1(void *);
void *processi2(void *);


int main(int argc, char const *argv[])
{
	int i;
	pthread_t proc1[N_PROCESSI_UNO], proc2[N_PROCESSI_DUE];

	srand(time(NULL));
	init();

	for(i=0;i<N_PROCESSI_DUE;i++){
		pthread_create(&proc2[i],NULL,processi2,(void *)&i);
		sleep(1);
	}

	for(i=0;i<N_PROCESSI_UNO;i++){
		pthread_create(&proc1[i],NULL,processi1,(void *)&i);
		sleep(1);
	}


	for(i=0;i<N_PROCESSI_UNO;i++){
		pthread_join(proc1[i],NULL);
	}

	return 0;
}

void *processi1(void *arg){
	int voto;
	int id = *(int *)arg;

	while(1){

		sem_wait(&sem_num_votanti1); 
		voto = rand()%N_PROCESSI_DUE; 
		voti_uno[voto]++;
		printf("\nINSIEME **1**: ID %d ho votato %d",id,voto);
		num_votanti_uno--;
		if(num_votanti_uno == 0){ //se sei l'ultimo a votare
			printf(" E SONO L'ULTIMO\n");
			winner2 = findMax(voti_uno,N_PROCESSI_DUE);
			printf("INSIEME **1**: VINCITORE: [%d]\n\n",winner2);
			num_votanti_uno = N_PROCESSI_UNO;
			initVoti(voti_uno,N_PROCESSI_DUE);
			pthread_cond_broadcast(&cond2); //avvisa l'altro gruppo
		}
		else 
			sem_post(&sem_num_votanti1); //sblocca il semaforo, lo fanno tutti tranne l'ultimo, cosi rimane bloccato



		pthread_mutex_lock(&mutex1);
		pthread_cond_wait(&cond1,&mutex1); //attendi la broadcast dall'altro lato
		if(id == winner1){
			printf("\nSONO IL VINCITORE DELL'INISEME **1**:[%d] %d\n",id,winner1);
			sem_post(&sem_num_votanti1);
		}

		pthread_mutex_unlock(&mutex1);

		sleep(2);
	}
}


void *processi2(void *arg){
	int voto;
	int id = *(int *)arg;

	while(1){
		

		pthread_mutex_lock(&mutex2);
		pthread_cond_wait(&cond2,&mutex2);
		if(winner2 == id){
			printf("\nSONO IL VINCITORE DELL'INSIEME --2--: [%d]\n",id);
			sem_post(&sem_num_votanti2);
		}

		pthread_mutex_unlock(&mutex2);

		sleep(2);

		sem_wait(&sem_num_votanti2);
		voto = rand()%N_PROCESSI_UNO;
		voti_due[voto]++;
		printf("\nINSIEME --2--: ID %d ho votato %d",id,voto);
		num_votanti_due--;

		if(num_votanti_due == 0){
			printf(" E SONO L'ULTIMO\n");
			winner1 = findMax(voti_due,N_PROCESSI_UNO);
			printf("INSIEME --2--: VINCITORE: [%d]\n\n",winner1);
			num_votanti_due = N_PROCESSI_DUE;
			initVoti(voti_due,N_PROCESSI_UNO);
			pthread_cond_broadcast(&cond1);
			
		}
		else 
			sem_post(&sem_num_votanti2);

		
	}
}

void init(){
	
	sem_init(&sem_num_votanti1,0,1);
	sem_init(&sem_num_votanti2,0,1);

	initVoti(voti_uno,N_PROCESSI_DUE);
	initVoti(voti_due,N_PROCESSI_UNO);

	mutex1 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	mutex2 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	cond1 = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
	cond2 = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

}

int findMax(int *a,int n){
	int i;
	int indice_max=-1,max = -1;

	for(i=0;i<n;i++){
		if(max<a[i]){
			max = a[i];
			indice_max = i;
		}
	}

	return indice_max;

}

void initVoti(int *a,int n){
	int i;
	for(i=0;i<n;i++)
		a[i] = 0;


}

