#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void *myFunction(void *arg);
int var=0;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
	pthread_t td1;
	int i;
	if(pthread_create(&td1,NULL,myFunction,NULL) != 0){
		printf("pthreade_create error\n");
		exit(-1);
	}

	for(i=0;i<20;i++){
		pthread_mutex_lock(&mymutex);
		var+=1;
		pthread_mutex_unlock(&mymutex);
		printf("o");
		fflush(stdout);
		sleep(1);
	}

	if(pthread_join(td1,NULL) != 0){
		printf("pthread_join error\n");
		exit(-2);
	}

	printf("\nValore var: %d\n", var);
	return 0;
}

void *myFunction(void *arg){
	int i,j;
	for(i=0;i<20;i++){
		pthread_mutex_lock(&mymutex);
		j=var;
		j+=1;
		printf(".");
		fflush(stdout);
		var = j;
		pthread_mutex_unlock(&mymutex);
		sleep(1);

	}
	return NULL;
}