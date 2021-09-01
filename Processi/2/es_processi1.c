#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	pid_t pid;
	int status;
	pid=fork();
	if(pid == 0){
		//execl("/bin/ls","ls","-l","pippo",NULL);
		execlp("ls","ls","-l","pippo",NULL);
		printf("execl fallita\n");
		exit(1);
	}

	else if (pid >0){
		pid = wait (&status);
		/* gestione stato */
		printf("terminato il figlio\n");
		exit(0);
	}

	else perror("fork");
	exit(2);
	return 0;
}