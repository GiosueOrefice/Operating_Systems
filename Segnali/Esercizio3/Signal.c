#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
/*
	Scrivere un programma C che con l'uso dei segnali sincronizzi un processo padre ed un processo figlio che
	scrivono e leggono,rispettivamente un numero intero alla volta(da 1 a N, dove N è passato da riga di comando)
	nella prima posizione di un file temporaneo opportunamente creato

*/

int n,filedes;
char readBuffer[1024];
pid_t pid;
int k;
char buffer[1024] = "io sto creando un file temporaneo e sto scrivendo";

void signalHandler(int signo);

int main(int argc, char const *argv[])
{
	if(argc != 2){
		printf("Inserisci n da riga di comando\n");
		exit(1);
	}

	n = atoi(argv[1]);
	if (n<1){
		printf("Devi inserire un intero positivo\n");
		exit(2);
	}

	char nomeBuff[20] = "provoXXXXXX";
	k=-n;
	printf("PID: %d\n\n",getpid());
	// Crea il file temporaneo, questa funzione sostituirà le "X"
	if((filedes = mkstemp(nomeBuff))<1){
		perror("mkstemp");
		exit(1);
	}


	// Chiama unlink in modo che ogni volta che il file viene chiuso o il programma esce
    // il file temporaneo viene cancellato
    unlink(nomeBuff);

    signal(SIGTSTP,signalHandler);

    if((pid=fork())<0){
    	perror("fork");
    	exit(3);
    }

    if(pid == 0){
    	kill(getppid(),SIGTSTP);
    }
  
  	while(1) pause();
    	

	return 0;
}

void signalHandler(int signo){
	

	if(pid > 0){
		k+=n;
		if(k>=strlen(buffer)){
	    	kill(pid,SIGINT);
	    	exit(1);
	    }

		if(lseek(filedes,0,SEEK_SET) == -1 ){
    	perror("lseek");
    	exit(4);
    } 
		if(-1 == write(filedes,&buffer[k],n)){
	        perror("write");
	        exit(5);
	    }
	    printf("PID: %d Scritto: ",getpid());
	    for(int i=0;i<n;i++){
	    	printf("%c",buffer[k+i]);
	    }
	    printf("\n");
	    kill(pid,SIGTSTP);
	    
	    	

	}

	else {
		if(lseek(filedes,0,SEEK_SET) == -1 ){
	    	perror("lseek");
	    	exit(6);
	    } 

	    int count;

		if((count =read(filedes,readBuffer,n))<0){
			perror("read");
			exit(7);
		}

		readBuffer[count] = '\0';
		printf("PID: %d letto: %s\n\n",getpid(),readBuffer);
    	bzero(readBuffer,1024);
    	kill(getppid(),SIGTSTP);
	}

	
}
