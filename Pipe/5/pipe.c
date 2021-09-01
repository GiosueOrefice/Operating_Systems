#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
/*
	
*/

int main(int argc, char const *argv[])
{

	int pfd[2];
	pid_t pid;

	if(argc != 3){
		printf("Inserire 2 comandi\n");
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
		execlp(argv[2],argv[2],NULL);
		perror("execlp");
		exit(-4);		
	}

	else {
		close(pfd[0]);
		dup2(pfd[1],1);
		close(pfd[1]);
		execlp(argv[1],argv[1],NULL);
		perror("execlp");
		exit(-3);

		
	}

	return 0;
}