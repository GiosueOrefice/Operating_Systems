#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define fifoName "/tmp/my_fifo"
int main(int argc, char const *argv[])
{
	int res,i;
	int open_mode = 0;
	int check = 1;
	if (argc <2 || argc>3){
		printf("Uso: Inserisci la modalità di apertura della FIFO\n");
		exit(1);
	}
	/* Impostiamo il valore di open_mode dagli argomenti */
	for(i=1;i<argc;i++){
		if(check){
			if(strncmp(argv[i],"O_RDONLY",8) == 0){
				open_mode |= O_RDONLY;
				check = 0;
			}
			else if(strncmp(argv[i],"O_WRONLY",8) == 0){ 
				open_mode |= O_WRONLY;
				check = 0;
			}
		}

			if(strncmp(argv[i],"O_NONBLOCK",8) == 0) open_mode |= O_NONBLOCK;

	}

	/*Se la FIFO non esiste la creiamo. Poi viene aperta */
	if(access(fifoName,F_OK) == -1){ 
	/*verifica l'accessibilità del real User-ID, mode può essere R_OK,W_OK,X_OK e F_OK*/
		res = mkfifo(fifoName,0777);
		if (res != 0){
			printf("Non posso creare la FIFO %s\n",fifoName);
			exit(2);
		}

	}
	printf("Processo %d apre la FIFO\n\n",getpid());
	res = open(fifoName,open_mode);
	printf("Risultato processo %d: %d\n",getpid(),res);
	sleep(6);
	if(res != -1) close(res);
	printf("Processo %d terminato\n",getpid());

	return 0;
}