#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#define SEM1 "/sem1"
#define SEM2 "/sem2"

/*
	Scrivere un programma che crea un processo figlio, e, alternandosi con il padre, 
	stampino gli elementi in una directory. Una volta finito, il figlio aspetta il segnale SIGUSR1
	ed invia al padre il numero di stampe effettuate e successivamente termini.
*/

int readDIR(sem_t *sem1,sem_t *sem2);
void signalHandler(int signo);

int cont;
int pfd[2];

int main(int argc, char const *argv[])
{
	
	pid_t pid;

	sem_t *sem1,*sem2;
	sem_unlink(SEM1);
	sem_unlink(SEM2);


	if((sem1 = sem_open(SEM1,O_CREAT,0777,1)) == NULL){
		printf("error");
		exit(-1);
	}

	if((sem2 = sem_open(SEM2,O_CREAT,0777,0)) == NULL){
		printf("error");
		exit(-1);
	}


	if(pipe(pfd) < 0){
		perror("pipe");
		exit(-4);
	}
	

	if((pid = fork())<0){
		perror("fork");
		exit(-5);
	}

	

	
	if(pid == 0){
		signal(SIGUSR1,signalHandler);
		close(pfd[0]);
		cont = readDIR(sem1,sem2);
		sleep(1);
		printf("FIGLIO PID: %d ha stampato %d volte\n",getpid(),cont);
		pause();

	}
		

	else {
		readDIR(sem2,sem1);
		sleep(3);
		kill(pid,SIGUSR1);
		close(pfd[1]);
		read(pfd[0],&cont,sizeof(cont));
		printf("PADRE PID: %d ha ricevuto il valore %d\n",getpid(),cont);

	}
	

	sem_close(sem1);
	sem_close(sem2);




	

	return 0;
}

int readDIR(sem_t *sem1,sem_t *sem2){

	int i,cont=0;
	char nameDirectory[1024];
	struct dirent *dirent;
	struct stat info;

	getcwd(nameDirectory,1024);

	DIR *dir = opendir(nameDirectory);
	if(dir == NULL){
		printf("opendir error\n");
		exit(-1);
	}
	sem_getvalue(sem1,&i);

	while(((dirent = readdir(dir)) != NULL)){
		
		i++;

		if(i%2 == 0)
			continue;

		cont++;
		sem_wait(sem1);
		printf("PID %d: %s\n",getpid(),dirent->d_name);
		sem_post(sem2);

	}

	return cont;
}

void signalHandler(int signo){

	write(pfd[1],&cont,sizeof(cont));
	exit(1);
}