#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define NUM_CARATTERI 60

struct data{
	char character;
	int fd;
	int pos;
	int num_char;
	int cont;
};


struct data x;


void* function(void* parameters){
	struct data *dati = (struct data*)parameters;
	int i;
	char c;
	int cont=0;
	printf("THREAD %lu\n",pthread_self());
	for(i=0;i<dati->num_char;i++){
		read(dati->fd,&c,sizeof(char));
		if(c == dati->character || tolower(c) == dati->character)
			cont++;
		printf("%c",c);
	}
	printf("\nHo trovato %d occorrenze\n\n",cont);
	dati->cont += cont;
	dati->pos += dati->num_char;
}

int main(int argc, char const *argv[])
{
	int N=4;
	pthread_t tid[N];
	int i,erro,fd;

	if((fd=open("Prova.txt",O_RDONLY)) <1){
		perror("open");
		exit(-1);
	}

	x.character = 'o';
	x.fd = fd;
	x.pos = 0;
	x.num_char = NUM_CARATTERI/N;
	x.cont = 0;

	if((erro =pthread_create(&tid[0],NULL,function,&x))!=0){
			printf("ERROR with code %d\n",erro);
			exit(-1);
		}

	for(i=1;i<N;i++){
		pthread_join(tid[i-1],NULL);
		if((erro =pthread_create(&tid[i],NULL,function,&x))!=0){
			printf("ERROR with code %d\n",erro);
			exit(-1);
		}

	}
	
	

	pthread_join(tid[N-1],NULL);
	printf("NUMERO OCCORRENZE %d\n",x.cont);

	return 0;
}