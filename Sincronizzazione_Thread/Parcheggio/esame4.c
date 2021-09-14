#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define MAX_POSTI 6
#define MAX 3
#define NUM_THREAD 15

/*
Un parcheggio possiede due accessi o corridoi, uno per consentire esclusivamente l'ingresso e l'altro l'uscita delle auto. 
Si prevede un numero massimo di posti auto individuati da striscie gialle presenti nel parcheggio. 
Ogni accesso può, in un certo istante, permettere il passaggio in ingresso ed in uscita ad al massimo un certo numero di auto pari a Max. 
In un dato istante gruppi di auto che stanno passando attraverso un corridoio sono tutti nello stesso verso e se il corridoio e pieno, 
si ritarda ogni acceaso a richieste ulteriori. Le auto possono anche presentarsi in gruppi (di consistenza numerica inferiori a Max). 
Si noti che il gruppo entra tutto o aspetta: non è possibile quindi il passaggio frazionario.

*/

int cont_auto_parcheggiate = 0;
int cont_auto_entrata = 0;
int cont_auto_uscita = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; //mutex per auto parcheggiate
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; // mutex per auto in entrate
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; //mutex per auto in uscita

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

void *autoFunction(void *);

int main(int argc, char const *argv[])
{
	int i;
	pthread_t tidAuto[NUM_THREAD];

	srand(time(NULL));
	for(i=0;i<NUM_THREAD;i++){
		pthread_create(&tidAuto[i],NULL,autoFunction,NULL);
	}

	for(i=0;i<NUM_THREAD;i++){
		pthread_join(tidAuto[i],NULL);
	}

	return 0;
}

void *autoFunction(void *arg){

	int num_auto = 1 + rand()%MAX;
	sleep(1+rand()%5);
	printf("SIAMO %d AUTO CHE VOGLIONO ENTRARE %lu\n",num_auto,pthread_self());
	pthread_mutex_lock(&mutex2);
	while(cont_auto_entrata + num_auto > MAX){ //controllo auto in entrata
			pthread_cond_wait(&cond2,&mutex2);
	}

	cont_auto_entrata += num_auto;
	printf("\n--IN ENTRATA [%d] AUTO - TOT AUTO IN ENTRATA [%d] %lu--\n",num_auto,cont_auto_entrata,pthread_self());
	pthread_mutex_unlock(&mutex2);


	sleep(1+rand()%3);

	pthread_mutex_lock(&mutex1);

	while(cont_auto_parcheggiate + num_auto >MAX_POSTI){ //controllo auto parcheggiate
			pthread_cond_wait(&cond1,&mutex1);
	}

	cont_auto_parcheggiate += num_auto;
	printf("\n**PARCHEGGIATE {%d} AUTO - TOT AUTO PARCHEGGIATE {%d}%lu**\n",num_auto,cont_auto_parcheggiate,pthread_self());
	pthread_mutex_unlock(&mutex2);
	cont_auto_entrata -= num_auto;
	pthread_mutex_unlock(&mutex2);
	pthread_cond_broadcast(&cond2);
	pthread_mutex_unlock(&mutex1);


	sleep(1+rand()%5);

	pthread_mutex_lock(&mutex3);
	while(cont_auto_uscita + num_auto > MAX){ //controllo auto in uscita
		pthread_cond_wait(&cond3,&mutex3); 
	}

	cont_auto_uscita += num_auto;
	printf("\n@@USCENDO (%d) AUTO - TOT AUTO USCENTI (%d) %lu@@\n",num_auto,cont_auto_uscita,pthread_self());
	pthread_mutex_unlock(&mutex3);

	pthread_mutex_lock(&mutex2);
	cont_auto_parcheggiate -= num_auto;
	pthread_mutex_unlock(&mutex2);

	pthread_cond_broadcast(&cond1);
	sleep(1+rand()%3);

	pthread_mutex_lock(&mutex3);
	cont_auto_uscita -= num_auto;
	

	printf("\n++USCITE (%d) AUTO %lu++\n",num_auto,pthread_self());
	
	pthread_mutex_unlock(&mutex3);
	pthread_cond_broadcast(&cond3);
	return NULL;

}