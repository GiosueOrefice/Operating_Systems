#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void *myFunction(void *);
void initMat();

#define N 10
struct info
{
	char key;
	int x;
	int y;
}data = {'c',-1,-1};

char mat[N][N];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
	pthread_t tid[N];
	int i;
	srand(time(NULL));
	initMat();

	for(i=0;i<N;i++){
		pthread_create(&tid[i],NULL,myFunction,(void *)&i);
	}

	


	for(i=0;i<N;i++)
		pthread_join(tid[i],NULL);

	printf("TROVATO A %d %d\n",data.x,data.y);
	return 0;
}

void *myFunction(void *arg){
	int *i = (int * )arg;
	int j;
	for(j=0;j<N;j++){
		pthread_mutex_lock(&mutex);
		if(data.x != -1 && data.y != -1){
			pthread_mutex_unlock(&mutex);
			pthread_cancel(pthread_self());
			return NULL; 
		}
		if(data.key == mat[*i][j]){
			data.x=*i;
			data.y=j;
			printf("THREAD %lu TROVATO A %d %d\n",pthread_self(),data.x,data.y);
		}
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

void initMat(){
	int i,j;
	printf("  ");
	for(i=0;i<N;i++)
		printf("%d ",i);
	printf("\n");
	for(i=0;i<N;i++){
		printf("%d ",i);
		for(j=0;j<N;j++){
			mat[i][j]=97+rand()%26;
			printf("%c ",mat[i][j]);
		}
		printf("\n");
	}

}