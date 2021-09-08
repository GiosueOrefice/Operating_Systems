#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

/*
	Un sistema è composto di un buffer di N posizioni, 2*N processi attivi ed un processo coordinatore. 
	Il processo coordinatore estrae uno dopo l'altro in maniera casuale i numeri da 1 a N e ad ogni estrazione 
	i processi competono per aggiudicarsi l'accesso alla posizione corrispondente del buffer, 
	scrivendone il proprio PID.
	Un processo cha ha scritto il proprio PID nel buffer non partecipa più alla contesa. 
	Quando tutte le posizioni del buffer sono state assegnate, il coordinatore si alterna con l’ultimo processo 
	che ha avuto accesso al buffer nel leggere e stampare, uno alla volta, il contenuto del buffer.

	UTILIZZATI I THREAD INVECE CHE I PROCESSI
*/
#define N 5
unsigned long buffer[N];
int num_estratto=-1;
int cont_processi = 0;
int indice=0;

sem_t estrai,num_estratto_sem,prod_processo, prod_cordinatore;

void *functionProcesso(void *);
void *functionCordinatore(void *);

int main(int argc, char const *argv[])
{
	pthread_t processi[2*N];
	pthread_t cordinatore;
	int i;

	sem_init(&estrai,0,1);
	sem_init(&num_estratto_sem,0,0);
	sem_init(&prod_processo,0,0);
	sem_init(&prod_cordinatore,0,1);


	for(i=0;i<2*N;i++){
		pthread_create(&processi[i],NULL,functionProcesso,NULL);
	}

	pthread_create(&cordinatore,NULL,functionCordinatore,NULL);

	pthread_join(cordinatore,NULL);
	sleep(1);


	return 0;
}

void *functionCordinatore(void *arg){

	int cont=0;
	while(cont<N){
		sem_wait(&estrai); //puoi estrarre
		num_estratto++; //dovrebbe farlo in modo random ma può darsi che più volte estragga lo stesso numero
		sem_post(&num_estratto_sem); //segnala che un thread può inserire il proprio id
		cont++;
	}

	sleep(1);

	printf("\nSTAMPA ALTERNATA TRA IL CORDINATORE E L'ULTIMO THREAD\n\n");
	while(1){
		sem_wait(&prod_cordinatore); // è il turno del cordinatore per stampare
		printf("%d\t%lu CORDINATORE\n",indice,buffer[indice]);
		indice++;
		if(indice == N-1){ //se è la penultima stampa termino perchè l'ultima la farà il processo
				sem_post(&prod_processo);
				return NULL;
			}
			else if(indice == N){ // se è l'ultima esci
				return NULL;
			}
			else 
				sem_post(&prod_processo);
	}
}

void *functionProcesso(void *arg){

	sem_wait(&num_estratto_sem);
	buffer[num_estratto] = pthread_self();
	printf("%d\t%lu PROCESSO CON ID %lu\n",num_estratto,buffer[num_estratto],pthread_self());
	cont_processi++;
	if(cont_processi<N)
		sem_post(&estrai);

	else if( cont_processi == N){
		while(1){
			sem_wait(&prod_processo);
			printf("%d\t%lu PROCESSO CON ID %lu\n",indice,buffer[indice],pthread_self());
			indice++;
			if(indice == N-1){
				sem_post(&prod_cordinatore);
				break;
			}
			else if(indice == N)
				break;
			
			else 
				sem_post(&prod_cordinatore);
		}
	}

	return NULL;

}