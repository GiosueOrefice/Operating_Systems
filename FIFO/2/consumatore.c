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
	int open_mode = O_RDONLY;
	char buffer[BUFFER_SIZE];
	int bytes_read = 0;

	if(access(fifoName,F_OK) == -1){
		if((res = mkfifo(fifoName,0777)) <0){
			perror("mkfifo");
			exit(-1);
		}

	}

	printf("Process %d aperto IN LETTURA\n",getpid());

	if((pipe_fd = open(fifoName,open_mode))< 0){
		perror("open");
		exit(-2);
	}

	do{
		if((res = read(pipe_fd,buffer,BUFFER_SIZE)) < 0){
			perror("read");
			exit(-3);
		}
		bytes_read += res;
	}while(res>0);

	close(pipe_fd);
	printf("Process %d finished with %d bytes read\n",getpid(),bytes_read);
	return 0;
}