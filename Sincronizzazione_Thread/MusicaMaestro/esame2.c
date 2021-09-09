#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

/*
	Nella sala prove del giovane talentuoso chitarrista Matteo Mancuso possono entrare e trovare posto seduto al più
	N appassionati chitarristi per una Masterclass con Matteo. I partecipanti rimasti in piedi seguiranno il seminario
	successivo. Ogni allievo che ha trovato posto può richiedere ad un collaboratore di Matteo Mancuso un modello di
	chitarra degli M modelli messi a disposizione dalla casa produttrice, di cui Matteo Mancuso è testimonial,
	considerando che per ogni modello ci sono K chitarre. Quando tutti gli N allievi hanno la propria chitarra, Matteo
	suona un pezzo semplice chiedendo a tutti gli allievi di suonarlo insieme a lui. La Masterclass termina quando non
	ci sono più chitarristi in attesa.
*/

#define N 4
#define M 3
#define K 3
#define NUM_ALLIEVI 3*N
#define num_pezzi 3
int posti=0;
int chitarre[M][K];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
sem_t sem;
int cont_allievi_ok = 0;
char pezzo[1024];
int check[num_pezzi];
int i_chitarre = 0,j_chitarre = 0;

void *allievo(void *);
void *marco(void *);
void init();

int main(int argc, char const *argv[])
{
	int i;
	pthread_t allievi[NUM_ALLIEVI];
	pthread_t tidMarco;
	sem_init(&sem,0,1);
	init();

	for(i=0;i<NUM_ALLIEVI;i++){
		pthread_create(&allievi[i],NULL,allievo,NULL);
	}

	pthread_create(&tidMarco,NULL,marco,NULL);

	pthread_join(tidMarco,NULL);
	for(i=0;i<NUM_ALLIEVI;i++)
		pthread_join(allievi[i],NULL);

	return 0;
}

void *allievo (void *arg){
	int riga,colonna,j;
	while(1){
		pthread_mutex_lock(&mutex);
		if(posti <N){
			posti++;
			pthread_mutex_unlock(&mutex);
			break;
		}
		else {
			pthread_cond_wait(&cond,&mutex);
			pthread_mutex_unlock(&mutex);
		}
	}

	pthread_mutex_lock(&mutex2);
	chitarre[i_chitarre][j_chitarre] = -1;
	printf("HO PRESO LA CHITARRA [%d] [%d]\n",i_chitarre,j_chitarre);
	
	j_chitarre = (j_chitarre+1)%M;
	if(j_chitarre == 0)
		i_chitarre = (i_chitarre+1)%M;
	pthread_mutex_unlock(&mutex2);

	pthread_mutex_lock(&mutex3);
	cont_allievi_ok++;
	if(cont_allievi_ok == N){
		cont_allievi_ok = 0;
		pthread_cond_broadcast(&cond2);

	}
	else {
		pthread_cond_wait(&cond2,&mutex3);
		
	}
	pthread_mutex_unlock(&mutex3);

	for(j=0;j<num_pezzi;j++){
		pthread_mutex_lock(&mutex2);

		if(!check[j]){
			pthread_cond_wait(&cond3,&mutex2);
			
		}
		cont_allievi_ok++;
		printf("%s\n",pezzo);
		if(cont_allievi_ok == N){
			cont_allievi_ok =0;
			printf("\n");
			sem_post(&sem);
		}


		pthread_mutex_unlock(&mutex2);


	}

	return NULL;



}

void *marco(void *arg){
	int j,i;
	char canzoni[num_pezzi][1024] = {"ciao","buonasera","buonanotte"};
	for(i=0;i<3;i++){

		printf("TURNO N° %d\n",i+1);

		for(j=0;j<num_pezzi;j++){
			sem_wait(&sem);
			pthread_mutex_lock(&mutex2);
			bzero(pezzo,1024);
			strcpy(pezzo,canzoni[j]);
			check[j] = 1;
			pthread_cond_broadcast(&cond3);
			pthread_mutex_unlock(&mutex2);

		}
		sleep(2); 
		init();
		pthread_cond_broadcast(&cond);
	}

}

void init(){
	int i,j;
	for(i=0;i<M;i++){
		for(j=0;j<K;j++)
			chitarre[i][j]=1;
	}

	for(i=0;i<num_pezzi;i++)
		check[i] = 0;

	posti = 0;

}