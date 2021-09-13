#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LVLMAX 2

void alberoTernario (int level);
void foglie(int level);

int main(int argc, char *argv[])
{

	printf("SONO IL PROCESSO %d\n\n",getpid());
	alberoTernario(0);
	
	return 0;	
}

void alberoTernario (int level){
	int status,i;
	pid_t pid[3];

	if (level == LVLMAX)
		foglie(level);


	for(i=0;i<3;i++){
		if((pid[i] = fork())<0){
			perror("fork");
			exit(level);
		}
		if(pid[i] == 0){
			alberoTernario(level+1);
			exit(level+1);
		}
	}

	for(i=0;i<3;i++)
		waitpid(pid[i],&status,WUNTRACED);

	sleep(level);
	printf("Concluso, sono il processo %d figlio di %d al livello %d\n",getpid(),getppid(),level);
}

void foglie (int level){
	sleep(level);
	printf("Concluso! sono il processo %d figlio di %d FOGLIA LV %d\n",getpid(),getppid(),level);
	exit(level);
}