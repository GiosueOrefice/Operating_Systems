#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	printf("sono il processo %d\n",getpid());
	execl("/bin/ls","ls","ESERCIZI",NULL);
	printf("questa non stamperà mai\n");

	return 0;
}