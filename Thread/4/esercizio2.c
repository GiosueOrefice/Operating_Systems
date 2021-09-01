#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 12
int key;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
int risposte[2];
int contRisposte=0;
int checkRisposta = 0;

void *myFunction (void *arg);

int main(int argc, char const *argv[])
{
	int i;
	pthread_t tid[N];

	srand(time(NULL));

	key = 1 + rand()%20;
	printf("KEY %d\n",key);
	for(i=0;i<N;i++)
		pthread_create(&tid[i],NULL,myFunction,NULL);

	int tentativi = N;
	while(tentativi){
		pthread_mutex_lock(&mutex);
		

		contRisposte++;
		checkRisposta =1;
		printf("THREAD AUMENTA CONTRISPOSTE %d\n",contRisposte);
		
		pthread_cond_broadcast(&cond1);
		

		while(contRisposte == 10 ){
			pthread_cond_wait(&cond2,&mutex);
		}

		if(contRisposte>0)
			pthread_cond_broadcast(&cond1);

		pthread_mutex_unlock(&mutex);
		tentativi--;
		
	

	}

	return 0;
}

void *myFunction(void *arg){
	int risp;

	pthread_mutex_lock(&mutex);
	while(checkRisposta != 1){
		pthread_cond_wait(&cond1,&mutex);
		
	}
	printf("THREAD USCITO DAL PRIMO WHILE\n");
	contRisposte--;
	
		checkRisposta = 0;
		pthread_cond_signal(&cond2);
	

	pthread_mutex_unlock(&mutex);
	return NULL;




}