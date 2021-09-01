#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
	Scrivere un programma C che riceva in input da tastiera due numeri interi, a e b, e ne stampi a video:
		la somma "a+b" solo quando riceve il segnale SIGUSR2
		la differenza "a-b" quando riceve il segnale SIGUSR1	
		il programma esce quando riceve SIGINT

	NB: Per poter inviare un segnale da un altra shell: kill -s NOME_SEGNALE PID_PROCESSO

*/

void signalHandler(int signo);
int a,b;

int main(int argc, char const *argv[])
{
	printf("SONO IL PROCESSO %d\n",getpid());
	printf("Inserisci due numeri interi:\n");
	scanf("%d %d",&a,&b);
	signal(SIGUSR2,signalHandler);
	signal(SIGUSR1,signalHandler);
	signal(SIGINT,signalHandler);
	while(1);
	return 0;
}

void signalHandler(int signo){
	if(signo == SIGUSR2)
		printf("SEGNALE SIGUSR2 RICEVEUTO\nSOMMA: %d\n",a+b);

	else if (signo == SIGUSR1)
		printf("SEGNALE SIGUSR1 RICEVEUTO\nSOMMA: %d\n",a-b);

	else 
		exit(1);

}