#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


/*
	In un sistema sono attivi N processi dotati di un credito C. 
	Ogni processo genera un numero casuale P in [1..C] e verifica se il proprio P è maggiore di quello degli altri processi (a parità di P vince il processo con PID minore). 
	Il processo vincitore (quello con P maggiore) aggiunge al proprio credito C le quantità P generate da tutti gli altri
	processi che invece sottragono al proprio credito C la quantità P generata. Un processo con credito C=0 termina.
*/

#define C 20
#define N 5

int credito[N];
int P[N];
int num_processi_ok = N;
int cont_processi_perdenti= 0; 
unsigned long tid[N];

struct info {
	int max_indice;
	unsigned long tidMax;
	int max;
} infoMax; //struct per le info relative al calcolo del massimo

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *esecuzione(void *arg);
void calcolaMax (int );
void lastThreadCheck();
void initInfoMax();

int main(int argc, char const *argv[])
{
	
	int i;
	pthread_t processi[N];

	srand(time(NULL));
	initInfoMax();
	printf("\tCREDITO DI TUTTI\n ");
	for(i=0;i<N;i++)
		printf("%d\t",i);

	printf("\n ");

	for(i=0;i<N;i++){
		credito[i]=C;
		printf("%d \t",credito[i]);

	}
	printf("\n\n");

	for(i=0;i<N;i++){
		pthread_create(&processi[i],NULL,esecuzione,(void * )&i);
		sleep(1);
	}

	for(i=0;i<N;i++){
		pthread_join(processi[i],NULL);
	}

	return 0;
} //fine MAIN

void *esecuzione(void *arg){
	int *indice = (int *)arg;
	int i = *indice;
	int j;

	tid[i] = pthread_self(); 
	sleep(N-i); 
	
	while(1){
		P[i] = 1+ rand()%C;  //scegli numero casuale
		
		pthread_mutex_lock(&mutex);
		if(cont_processi_perdenti == N-1){ //se sei l'ultimo thread in vita, hai vinto
			printf("**HO VINTO  %d %lu**\n",i,pthread_self());
			return NULL;
		}

		num_processi_ok--;
		printf("VALORE ESTRATTO DA %d: %d\n",i,P[i]);
		if(num_processi_ok - cont_processi_perdenti>0){  //se hai estratto il numero ma altri ancora no, attendi
			calcolaMax(i);
			pthread_cond_wait(&cond,&mutex);
			pthread_mutex_unlock(&mutex);
		}

		else if( num_processi_ok -cont_processi_perdenti == 0){ // se sei l'ultimo dei partecipanti in gioco ad aver estratto
			calcolaMax(i); // calcola max
			num_processi_ok = N - cont_processi_perdenti; //resetta il numero dei processi che hanno estratto
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond); //segnala a tutti che il max è stato calcolato
		}

		
		if(infoMax.max_indice == i){ //se sono il massimo
			
			for(j=0;j<N;j++){ //fai la somma di tutti i valori estratti e aggiungili al credito
				if(P[j] != -1)
					credito[i] += P[j];
			} 
			
			printf("SONO **%d** e sono il MAX, P[%d] = %d TID %lu\n",i,i,credito[i],pthread_self());
		}

		else { //se non lo sono
			credito[i] -= P[i];
			

			if(credito[i] <= 0){ //se ho perso
				printf("SONO %d e NON sono il MAX, P[%d] = %d TID %lu HO PERSO xxxx\n",i,i,credito[i],pthread_self());
				P[i] = -1;
				
				cont_processi_perdenti++;
				//CONTROLLA SE SONO L'ULTIMO PROCESSO A FARE LA SOTTRAZIONE, SE SI AVVISA GLI ALTRI E POI TERMINA
				lastThreadCheck();

				return NULL;
			}

			else  
				printf("SONO %d e NON sono il MAX, P[%d] = %d TID %lu\n",i,i,credito[i],pthread_self());
			
			

		}

		//CONTROLLA SE TUTTI HANNO FATTO LE DOVUTE OPERAZIONI DI SOMMA O SOTTRAZIONE
		lastThreadCheck();


		//sleep(1);
	}


}

void calcolaMax(int j){
	if(infoMax.max<P[j] || (infoMax.max == P[j] && infoMax.tidMax>tid[j])){
		infoMax.max = P[j];
		infoMax.max_indice = j;
		infoMax.tidMax = tid[j];
	}
}

void initInfoMax(){
	infoMax.max = -1;
	infoMax.max_indice = -1;
	infoMax.tidMax = 0;
}

void lastThreadCheck(){
	pthread_mutex_lock(&mutex2);
	num_processi_ok --;

	if( num_processi_ok == 0){
		printf("\n");
		num_processi_ok = N;
		initInfoMax();
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex2);


	}

	else {
		pthread_cond_wait(&cond,&mutex2);
		pthread_mutex_unlock(&mutex2);
	}

}