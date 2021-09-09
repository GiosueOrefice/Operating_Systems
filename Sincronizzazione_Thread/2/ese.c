#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

/*
	Si realizzi un programma in C che con l'ausilio della libreria Pthread
	- lancia n thread per calcolare la somma degli elementi di ciascuna riga di una matrice nxn di interi
	  generati casualmente in un intervallo [0,255],allocata dinamicamente

	- IL calcolo della somma degli elementi di ciascuna riga deve essere effettuato concorrentemente su tutte
	  le righe, secondo la seguente modalità:
	  - il thread i-esimo con i pari calcola la somma degli elementi di indice pari della i-esima riga
	  - il thread i-esimo con i dispari calcola la somma degli elementi di indice dispari della i-esima riga

	Calcolate le somme parziali, si provvederà a ricercarne il minimo ed a stamparlo a video
*/

#define N 6
struct info
{
	pthread_mutex_t mutex;
	int minimo;
	int *mat;
} dati = {PTHREAD_MUTEX_INITIALIZER,INT_MAX};

struct privateThread{
	int sum;
	int i;
};

void *myFunction(void *arg);
void riempiMat();
int main(int argc, char const *argv[])
{
	pthread_t tid[N];
	int i;
	struct privateThread pT[N];
	srand(time(NULL));

	riempiMat();
	for(i=0;i<N;i++){
		pT[i].sum=0;
		pT[i].i=i;
		pthread_create(&tid[i],NULL,myFunction,&pT[i]);
	}

	for(i=0;i<N;i++)
		pthread_join(tid[i],NULL);

	for(i=0;i<N;i++)
		printf("SOMMA PARZIALE %desima riga: %d\n",pT[i].i,pT[i].sum);

	printf("IL MINIMO E' %d \n",dati.minimo);
	return 0;
}

void *myFunction(void *Arg){
	struct privateThread *x = (struct privateThread *)Arg;
	int j=0;

	for(j=x->i%2;j<N;j+=2){
		x->sum+=*(dati.mat+x->i*N+j);
	}

	pthread_mutex_lock(&dati.mutex);
	if(dati.minimo>x->sum)
		dati.minimo=x->sum;
	pthread_mutex_unlock(&dati.mutex);

	return NULL;
}

void riempiMat(){
	int i,j;
	dati.mat = (int *) malloc(N*N*sizeof(int));
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			*(dati.mat+i*N+j) = rand()%256;
			printf("%d\t",*(dati.mat+i*N+j));
		}
		printf("\n");
	}
}