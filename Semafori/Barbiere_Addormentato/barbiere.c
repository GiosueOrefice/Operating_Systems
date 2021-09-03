#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

/*
	Un negozio di barbiere con:
		- 1 Barbiere;
		- 1 poltrona;
		- N sedie per clienti in attesa;

	Se vi sono clienti nel negozio il barbiere dorme sulla poltrona
	Il primo cliente che entra nel negozio vuoto sveglia il barbiere
	I clienti che entrano trovando la poltrona occupata si mettono in attesa su una sedia
	Il cliente che non trova una sedia libera va a cercare un altro barbiere senza attendere
*/
#define N 30

sem_t sveglia,poltrona;
int clienti_nel_salone=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex per scrivere e leggere la variabile clienti_nel_salone

unsigned long tid; //variabile utilizzata per salvare quale cliente sta tagliando i capelli
sem_t sem_print_tid; //semaforo per gestire la scrittura e lettura della variabile tid


void *cliente(void *arg);
void *barbiere(void *arg);


int main(int argc, char const *argv[])
{
	int numClienti = 100;
	pthread_t clientiT[numClienti],barbiereT;
	int i;

	srand(time(NULL));

	sem_init(&sveglia,0,0);
	sem_init(&poltrona,0,0);
	sem_init(&sem_print_tid,0,0);

	pthread_create(&barbiereT,NULL,barbiere,NULL);

	for(i=0;i<numClienti;i++){
		pthread_create(&clientiT[i],NULL,cliente,NULL);
		sleep(1);
	}

	for(i=0;i<numClienti;i++)
		pthread_join(clientiT[i],NULL);

	sem_destroy(&sem_print_tid);
	sem_destroy(&sveglia);
	sem_destroy(&poltrona);

	return 0;
}

void *cliente(void *arg){
	sleep(1+rand()%4);
	pthread_mutex_lock(&mutex);
	if(clienti_nel_salone >= N+1){ //se la poltrona e le N sedie sono occupate vai via
		printf("CLIENTE: --SEDIE TUTTE OCCUPATE, ARRIVEDERDI DA %lu--\n",pthread_self());
		pthread_mutex_unlock(&mutex);
		return NULL;
	}

	printf("CLIENTE: ARRIVATO NEL SALONE %lu\n",pthread_self());
	if(clienti_nel_salone == 0) // se il salone è vuoto sveglia il barbiere
		sem_post(&sveglia);

	clienti_nel_salone++;
	pthread_mutex_unlock(&mutex);
	sem_wait(&poltrona);
	tid = pthread_self(); 
	sem_post(&sem_print_tid);
	sleep(3); //tagliando i capelli
	printf("\nCLIENTE: **HO TAGLIATO I CAPELLI, ARRIVEDERCI DA %lu**\n", pthread_self());
	return NULL;
}


void *barbiere(void *arg){

	while(1){
		pthread_mutex_lock(&mutex);
		if(clienti_nel_salone == 0){ //se il salone è vuoto dormi
			printf("BARBIERE: ATTENDO CHE ARRIVI UN CLIENTE\n");
			pthread_mutex_unlock(&mutex);
			sem_wait(&sveglia);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&poltrona); //permetti al cliente di sedersi
		sem_wait(&sem_print_tid);
		printf("\nBARBIERE: **STO TAGLIANDO I CAPELLI A %lu**\n",tid);
		sleep(3); //tagliando i capelli
		pthread_mutex_lock(&mutex);
		clienti_nel_salone--;
		pthread_mutex_unlock(&mutex);
		sleep(2);

	}
}