#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

/*
Realizzare la simulazione di un incrocio stradale regolato da un semaforo. 
Le automobili sono rappresentate da processi e il semaforo è regolato tramite un gestore.
Il semaforo può assumere soltanto i valori rosso o verde.
Quando un’auto arriva a uno dei quattro lati dell’incrocio, chiama la funzione arriva(lato) dove lato può assumere i valori 0,1,2,3. 
Se il semaforo dal lato in cui è arrivata l’automobile è rosso, la funzione si blocca in attesa che diventi verde, altrimenti ritorna immediatamente. 
Quando l’auto lascia l’incrocio chiama la funzione lascia().
Scrivere i processi auto e gestore, rispettando i seguenti vincoli:
a) Il semaforo passa da rosso a verde e viceversa ogni 5 secondi;
b) Il semaforo non può cambiare da rosso a verde se ci sono auto che impegnano l’incrocio.

*/
#define N 20
int check=0;
int terminati_5_secondi = 0;
int num_auto = 0;
int lato_attivo = -1;

pthread_mutex_t mutex_check = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_terminati = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_num_auto = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lato = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_riprova = PTHREAD_MUTEX_INITIALIZER;
sem_t primo_passaggero,avvisa,terminato;
pthread_cond_t riprova = PTHREAD_COND_INITIALIZER;

void *gestisci(void *arg);
void *guidare(void *arg);
void attraversa(int lato);

int main(int argc, char const *argv[])
{
	pthread_t gestore;
	pthread_t automobili[N];
	int i;
	srand(time(NULL));
	sem_init(&primo_passaggero,0,0);
	sem_init(&avvisa,0,0);
	sem_init(&terminato,0,0);

	pthread_create(&gestore,NULL,gestisci,NULL);

	for(i=0;i<N;i++){
		pthread_create(&automobili[i],NULL,guidare,NULL);
		sleep(1);
	}

	for(i=0;i<N;i++)
		pthread_join(automobili[i],NULL);


	return 0;
}

void *gestisci(void *arg){

	while(1){
		sem_wait(&avvisa);
		pthread_mutex_lock(&mutex_lato);
		printf("\nGESTORE: ACCENDO IL SEMAFORO N° %d\n",lato_attivo);
		pthread_mutex_unlock(&mutex_lato);
		
		sem_post(&primo_passaggero);
		sleep(5);
		pthread_mutex_lock(&mutex_terminati);
		terminati_5_secondi = 1;
		printf("GESTORE: TERMINATI I 5 SECONDI\n");
		pthread_mutex_unlock(&mutex_terminati);
		pthread_mutex_lock(&mutex_lato);
		lato_attivo = -1;
		pthread_mutex_unlock(&mutex_lato);
		pthread_mutex_lock(&mutex_num_auto);
		if(num_auto > 0){
			printf("GESTORE: CI SONO ANCORA  %d AUTO CHE STANNO ATTRAVERSANDO \n",num_auto);
			pthread_mutex_unlock(&mutex_num_auto);
			sem_wait(&terminato);
		}

		pthread_mutex_unlock(&mutex_num_auto);
		pthread_mutex_lock(&mutex_terminati);
		terminati_5_secondi = 0;
		pthread_mutex_unlock(&mutex_terminati);
		pthread_mutex_lock(&mutex_check);
		check = 0;
		pthread_mutex_unlock(&mutex_check);
		pthread_cond_broadcast(&riprova);
	}

	return NULL;
}

void *guidare(void *arg){
	int lato = 1 + rand()%4;
	while(1){
		pthread_mutex_lock(&mutex_check);
		if(check == 0){
			pthread_mutex_lock(&mutex_lato);
			lato_attivo = lato;
			pthread_mutex_unlock(&mutex_lato);
			check = 1;
			pthread_mutex_unlock(&mutex_check);
			sem_post(&avvisa);
			sem_wait(&primo_passaggero);
			attraversa(lato);
			
			break;

		}

		else {
			pthread_mutex_lock(&mutex_lato);
			if(lato_attivo == lato){
				pthread_mutex_unlock(&mutex_lato);
				pthread_mutex_unlock(&mutex_check);
				attraversa(lato);
				break;
				
			}

			else {

				pthread_mutex_unlock(&mutex_check);
				pthread_mutex_unlock(&mutex_lato);
				pthread_mutex_lock(&mutex_riprova);
				pthread_cond_wait(&riprova,&mutex_riprova);
				pthread_mutex_unlock(&mutex_riprova);
			}
		}
	}

	return NULL;
}

void attraversa(int lato){
	pthread_mutex_lock(&mutex_num_auto);
	num_auto++;
	printf("STO ATTRAVERSANDO NEL LATO %d %lu\n",lato,pthread_self());
	pthread_mutex_unlock(&mutex_num_auto);
								
	sleep(2+rand()%4);

	pthread_mutex_lock(&mutex_num_auto);
	num_auto--;
	printf("HO ATTRAVERSATO NEL LATO %d %lu\n",lato,pthread_self());
	pthread_mutex_lock(&mutex_terminati);
					 
	if(terminati_5_secondi == 1 && num_auto==0)
		sem_post(&terminato);
									
	pthread_mutex_unlock(&mutex_terminati);
	pthread_mutex_unlock(&mutex_num_auto);
}