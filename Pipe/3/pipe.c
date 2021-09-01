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
		execlp("sort","sort",NULL);
		perror("execlp");
		exit(-3);
		
	}

	else {
		close(pfd[0]);
		write(pfd[1],"IO STO PROVANDO\n",strlen("IO STO PROVANDO\n"));
		write(pfd[1],"BYE BYE\n",strlen("BYE BYE\n"));
		write(pfd[1],"CIAO A TUTTI\n",strlen("CIAO A TUTTI\n"));
		close(pfd[1]);
		waitpid(pid,NULL,0);
		
	}

	return 0;
}