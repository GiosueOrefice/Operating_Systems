#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/*
	Semplice protocollo di comunicazione tramite pipe anonima. 
	Dovendo inviare messaggi di lunghezza variabile ogni messaggio è preceduto da un intero che fornisce
	la sua lunghezza in caratteri. 
	Il figlio scrive e il padre legge.
	
*/

int main(int argc, char const *argv[])
{
	int pfd[2];
	pid_t pid;
	char *message[3] = {"Primo","Secondo","Terzo"};
	int len,i;
	if(pipe(pfd) == -1){
		perror("pipe");
		exit(-1);
	}

	if((pid = fork())<0){
		perror("fork");
		exit(-2);
	}

	if(pid == 0){
		close(pfd[0]);

		for(i=0;i<3;i++){
			len = strlen(message[i]) + 1;
			write(pfd[1],&len,sizeof(int));
			write(pfd[1],message[i],len);
		}
		close(pfd[1]);
	}

	else {
		close(pfd[1]);
		char buffer[1024];
		while(read(pfd[0],&len,sizeof(int))){
			read(pfd[0],&buffer,len);
			printf("Letto: %s\n", buffer);
			bzero(buffer,len);
		}
		close(pfd[0]);
	}

	return 0;
}