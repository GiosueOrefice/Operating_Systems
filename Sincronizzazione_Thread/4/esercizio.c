#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 6
int cont = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *myFunction(void *);

int main(int argc, char const *argv[])
{
	int i;
	pthread_t tid[N];
	srand(time(NULL));
	for(i=0;i<N;i++)
		pthread_create(&tid[i],NULL,myFunction,NULL);

	for(i=0;i<N;i++)
		pthread_join(tid[i],NULL);

	return 0;
}

void *myFunction(void *arg){
	int time = 1+ rand()%2;
	sleep(time);
	pthread_mutex_lock(&mutex);
	cont++;
	if(cont==N){
		pthread_cond_broadcast(&cond);
		printf("INVIATO SEGNALE %lu\n",pthread_self());

	}
	else {
		printf("ASPETTO %lu\n",pthread_self());
		pthread_cond_wait(&cond,&mutex);
		printf("ESCO %lu\n",pthread_self());


	}
	pthread_mutex_unlock(&mutex);

	return NULL;

}