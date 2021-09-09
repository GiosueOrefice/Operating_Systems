#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int var=0;
void* function(){
	int second = (int)1+rand()%10;
	sleep(second);
	var+=1;
	//printf("Valore var %d\n",var);
}

int main(int argc, char const *argv[])
{
	int N=1000;
	srand(time(NULL));
	pthread_t tid[N];
	int i,erro;

	for(i=0;i<N;i++){
		if((erro =pthread_create(&tid[i],NULL,function,NULL))!=0){
			printf("ERROR with code %d\n",erro);
			exit(-1);
		}
	}
	
	printf("Attendo che tutti i thread terminano..\n");
	for(i=0;i<N;i++)
		pthread_join(tid[i],NULL);

	printf("VALORE FINALE %d\n",var);

	return 0;
}