#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
/*
	Scrivere un programma che esegue il comando shell "ls -R | grep <path" dove 
	<pat> è un pattern inserito dall'utente
*/

int main(int argc, char const *argv[])
{

	int pfd[2];
	pid_t pid;

	if(argc !=2){
		printf("Inserire <pat>\n");
		exit(-5);
	}
	
	if(pipe(pfd) == -1){
		perror("pipe");
		exit(-1);
	}

	if((pid = fork())<0){
		perror("fork");
		exit(-2);
	}

	if(pid == 0){
		close(pfd[1]);
		dup2(pfd[0],0);
		close(pfd[0]);
		execlp("grep","grep",argv[1],NULL);
		perror("exec");
		exit(-3);
	}

	else {
		close(pfd[0]);
		dup2(pfd[1],1);
		close(pfd[1]);
		execlp("ls","ls","-R",NULL);
		perror("exec padre");
		exit(-4);
		
	}

	return 0;
}