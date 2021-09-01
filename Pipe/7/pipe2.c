#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

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

	int pfd2[2];
	if(pipe(pfd2) == -1){
		perror("pipe");
		exit(-1);
	}

	if((pid = fork())<0){
		perror("fork");
		exit(-2);
	}

	if(pid == 0){ /*FIGLIO */
		close(pfd[1]);
		dup2(pfd[0],0);
		close(pfd[0]);

		close(pfd2[0]);
		dup2(pfd2[1],1);
		close(pfd2[1]);
		execlp("sort","sort",NULL);
		perror("exec");
			
		
		
	}

	else { /*PADRE */

		if((pid = fork())<0){
			perror("fork");
			exit(-2);
		}

		if(pid != 0){
		close(pfd[0]);
		dup2(pfd[1],1);
		close(pfd[1]);
		close(pfd2[0]);
		close(pfd2[1]);
		execlp("ls","ls",NULL);
		perror("exec padre");
		exit(-4);
		}

		else { //FIGLIO
			close(pfd2[1]);
			dup2(pfd2[0],0);
			close(pfd2[0]);
			close(pfd[0]);
			close(pfd[1]);
			execlp("grep","grep",argv[1],NULL);
			perror("exec ");
			exit(-4);
			
		}
		
	}

	return 0;
}