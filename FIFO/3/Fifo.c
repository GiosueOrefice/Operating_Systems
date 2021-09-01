#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
	Scrivere un programma che gestisce il comportamento di un padre e due figlio (fpari e fdispari);
	Il padre legge numeri positivi da tastiera fino a che non arriva un numero negativo
		- se il numero è pari lo manda a fpari
		- se il numero è dispari lo manda a fdispari
 	I figli effettuano le somme parziali, quindi rimandono le somme al padre che calcola la somma totale e 
 	la stampa a video

*/


#define parentFifo  "/tmp/parent_fifo"

void childOperations(char *nameFifo);



int main(int argc, char const *argv[])
{
	char pariFifo[30] = "/tmp/pari_fifo";
	char dispariFifo[30] = "/tmp/dispari_fifo";
	pid_t pid;
	int byteSent;
	int pipe_fd,pari_fd,dispari_fd,fd;
	int sum = 0,x;

	if(access(parentFifo,F_OK) == -1){
		if(mkfifo(parentFifo,0777) < 0){
			perror("mkfifo parent");
			exit(-1);
		}
	}

	if(access(pariFifo,F_OK) == -1){
		if(mkfifo(pariFifo,0777) < 0){
			perror("mkfifo pari");
			exit(-1);
		}
	}

	if(access(dispariFifo,F_OK) == -1){
		if(mkfifo(dispariFifo,0777) < 0){
			perror("mkfifo dispari");
			exit(-1);
		}
	}

	if((pid = fork())<0){
		perror("fork");
		exit(-2);
	}

	if(pid == 0) //FIGLIO PARI
		childOperations(pariFifo);


	else {

		if((pid = fork())<0){
		perror("fork");
		exit(-2);
		}

		if(pid != 0){ //PADRE
			if((pari_fd = open(pariFifo,O_WRONLY))< 0){
				perror("open");
				exit(-2);
			}

			if((dispari_fd = open(dispariFifo,O_WRONLY))< 0){
				perror("open");
				exit(-2);
			}

			do {
				printf("Inserisci il numero:");
				scanf("%d",&x);
				if(x<0)
					break;

				if(x%2==0)
					fd = pari_fd;
				else 
					fd = dispari_fd;

				if(write(fd,&x,sizeof(x)) < 0){
					perror("write");
					exit(-3);
				}
			}while(1);
			close(dispari_fd);
			close(pari_fd);

			if((pipe_fd = open(parentFifo,O_RDONLY))< 0){
				perror("open");
				exit(-2);
			}

			//leggere le somme parziali
			while(read(pipe_fd,&x,sizeof(int)) >0)
				sum += x;
		
			close(pipe_fd);

			printf("PROCESS %d - SOMMA TOTALE %d\n",getpid(),sum);

		}

		else //FIGLIO DISPARI
			childOperations(dispariFifo);
			

		}	

	return 0;
}

void childOperations(char *nameFifo){
	int x,sum;
	int fd,pipe_fd;
	if((fd = open(nameFifo,O_RDONLY))< 0){
		perror("open");
		exit(-2);
	}

	while(read(fd,&x,sizeof(int)) >0)
		sum += x;

	if((pipe_fd = open(parentFifo,O_WRONLY))< 0){
		perror("open");
		exit(-2);
	}

	if(write(pipe_fd,&sum,sizeof(sum)) < 0){
		perror("write");
		exit(-3);
		}

	close(fd);
	close(pipe_fd);
	printf("PROCESS %d - SOMMA PARZIALE DEL PROCESSO  %d\n",getpid(),sum);
}