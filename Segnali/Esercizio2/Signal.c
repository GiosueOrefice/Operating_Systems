#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
	Scrivere un programma C che preso un argomento intero positivo da riga di comando, gestisca la seguente situazione:
		-genera due figli A e B e
			- se l'argomento è PARI invia un segnale SIGUSR1 alla ricezione del quale il figlio A calcola il cubo
			  del numero passato come argomento da linea di comando, mentre il figlio B stampa un messaggio di
			  arrivederci e termina
			- se l'argomento è DISPARI invia un segnale SIGUSR2 alla ricezione del quale il figlio B calcola il 
			  reciproco del numero passato come argomento, attende per un numero di secondi pari al doppio del 
			  numero passato come argomento ed invia un segnale SIGUSR2 al processo A dopodichè termina l'esecuzione.
			  Il figlio A, invece, attende la ricezione del segnale SIGUSR2, stampa un messaggio e termina

*/

void signalHandlerA(int signo);
void signalHandlerB(int signo);
pid_t pidA,pidB;
int x;
int main(int argc, char const *argv[])
{
	if (argc != 2) {
		fprintf(stderr,"Inserisci un numero intero positivo da linea di comando");
		exit (1);
	}

	
	if(argv[1][0] == '-'){
		fprintf(stderr,"Devi inserire un numero intero positivo");
		exit (1);
	} 

	x = atoi(argv[1]);
	if(x == 0)
		exit(1);
	printf("SONO IL PROCESSO %d\n",getpid());
	

	if((pidA = fork())<0)
		perror("fork");

	else if (pidA == 0){
		signal(SIGUSR1,signalHandlerA);
		signal(SIGUSR2,signalHandlerA);
		pause();
		exit(2);
	}

	if((pidB = fork())<0)
		perror("fork");

	else if(pidB == 0){
		signal(SIGUSR1,signalHandlerB);
		signal(SIGUSR2,signalHandlerB);
		pause();
		exit(3);
	}

		
	sleep(2);
	if(x%2 == 0){
		printf("\nPROCESSO %d INVIA IL SEGNALE SIGUSR1\n\n",getpid());
		kill(pidA,SIGUSR1);
		kill(pidB,SIGUSR1);
	}

	else {
		printf("\nPROCESSO %d INVIA IL SEGNALE SIGUSR2\n\n",getpid());
		kill(pidB,SIGUSR2);
	}



	int status;

	if(waitpid(pidA,&status,0)<0)
		perror("waitpid A");
	if(waitpid(pidB,&status,0)<0)
		perror("waitpid B");

	return 0;
}

void signalHandlerA(int signo){
	if(signo == SIGUSR1){
		printf("PID: %d -IL CUBO e': %d\n",getpid(),x*x*x);
	}
	else {
		printf("PID: %d -RICEVUTO SEGNALE SIGUSR2 DAL PROCESSO B\n",getpid());
	}
	
	exit(1);	
	
}

void signalHandlerB(int signo){
	if(signo == SIGUSR1)
		printf("PID: %d -ARRIVEDERCI\n",getpid());
	
	else {
		printf("PID: %d -IL RECIPROCO DI %d e' %2f\n",getpid(),x,(float)1/x);
		printf("PID: %d -ATTENDO %d SECONDI\n",getpid(),x*2);
		sleep(x*2);
		printf("PID: %d -INVIO SEGNALE SIGUSR2 AL PROCESSO A\n",getpid());
		kill(pidA,SIGUSR2);
	}

	exit(2);
}
