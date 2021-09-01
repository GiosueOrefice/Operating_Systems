#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/*
	Implementare la pipe $ ls | wc -w
	Il padre esegue una fork()
	il padre esegue una exec() per ls
	il figlio esegue una exec() per wc -w

	La exec non cambia la tavola dei descrittori ma si persono le variabili locali, per risolvere
	questo problema usiamo la dup2();

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
		close(pfd[0]);
		execlp("wc","wc","-w",NULL);
		perror("execlp");
		exit(-3);
	}

	else {
		close(pfd[0]);
		dup2(pfd[1],1);
		close(pfd[1]);
		execlp("ls","ls",NULL);
		perror("ls");
		exit(-4);
	}

	return 0;
}