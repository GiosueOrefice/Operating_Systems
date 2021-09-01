#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define fifoName "/tmp/my_fifo"
#define BUFFER_SIZE PIPE_BUF

int main(int argc, char const *argv[])
{
	int pipe_fd,res;
	int open_mode = O_WRONLY;
	int byte_sent=0;
	char buffer[BUFFER_SIZE];

	if(access(fifoName,F_OK) == -1){
		if((res = mkfifo(fifoName,0777)) <0){
			perror("mkfifo");
			exit(-1);
		}

	}

	printf("Process %d aperto IN SCRITTURA\n",getpid());

	if((pipe_fd = open(fifoName,open_mode))< 0){
		perror("open");
		exit(-2);
	}
	int cont=0;

	strcpy(buffer,"ESEMPIO PRODUTTORE CONSUMATORE\0");
	while(byte_sent<100){
		if((res = write(pipe_fd,buffer,strlen(buffer))) < 0){
			perror("write");
			exit(-3);
		}
		byte_sent += res;
	}

	close(pipe_fd);
	printf("Process %d finished with %d bytes sent\n",getpid(),byte_sent);
	return 0;
}