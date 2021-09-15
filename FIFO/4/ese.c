#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
	Processo prod. e cons.
	Il processo prod. genera un numero casuale da 1 a MAX per N volte e invio un intero per volta al processo cons.
	Il Processo cons. una volta ricevuti N interi fa la somma di questi interi e 
	invia la somma al processo produttore.
*/

#define MAX 10
#define N 3
#define FIFO_PROD "./fifoProd"
#define FIFO_CONS "./fifoCons"

int main(int argc, char const *argv[])
{
	pid_t consumatore;
	int cons,prod;
	int i;

	srand(time(NULL));

	unlink(FIFO_CONS);
	unlink(FIFO_PROD);

	if(access(FIFO_PROD,F_OK)<0){
		if(mkfifo(FIFO_PROD,0777)<0){
			perror("mkfifo");
			exit(-1);
		}

	}

	if(access(FIFO_CONS,F_OK)<0){
		if(mkfifo(FIFO_CONS,0777)<0){
			perror("mkfifo");
			exit(-1);
		}

	}



	if(!fork()){

		int val;
		int somma=0;

		if((cons = open(FIFO_CONS,O_WRONLY))<0){
			perror("open");
			exit(-1);
		}

		if((prod = open(FIFO_PROD,O_RDONLY))<0){
			perror("open");
			exit(-1);
		}

		for(i=0;i<N;i++){

			read(prod,&val,sizeof(int));
			somma += val;

		}
		printf("sommaaaaaaa %d\n",somma);
		write(cons,&somma,sizeof(int));


	}

	else {
		int val;
		int a;
		if((cons = open(FIFO_CONS,O_RDONLY))<0){
			perror("open");
			exit(-1);
		}

		if((prod = open(FIFO_PROD,O_WRONLY))<0){
			perror("open");
			exit(-1);
		}

		for(i=0;i<N;i++){
			val = 1 + rand()%MAX;
			printf("inviato %d\n",val);
			write(prod,&val,sizeof(int));
		}

		read(cons,&a,sizeof(int));
		printf("SOMMA %d\n",a);

	}

	
	return 0;
}