#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 4

sem_t sem;
int var=0;

void *myFunction(void *arg);

int main(int argc, char const *argv[])
{
	pthread_t tid[N];
	int i;

	sem_init(&sem,0,1);

	for(i=0;i<N;i++)
		pthread_create(&tid[i],NULL,myFunction,NULL);

	for(i=0;i<N;i++)
		pthread_join(tid[i],NULL);


	return 0;
}

void *myFunction(void *arg){
	int i;
	for(i=0;i<N;i++){
		sem_wait(&sem);
		var++;
		printf("Valore var %d thread %lu\n",var,pthread_self());
		sem_post(&sem);
		sleep(1);
	}
	return NULL;
}