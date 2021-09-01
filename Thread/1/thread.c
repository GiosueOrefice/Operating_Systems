#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct char_print_parms
{
	char character;
	int count;
};

void* char_print(void* parameters){
	struct char_print_parms* p = (struct char_print_parms*) parameters;
	int i;
	for(i=0;i<p->count;i++)
		printf("%c",p->character);

	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t tid1,tid2;
	struct char_print_parms tid1_args,tid2_args;

	tid1_args.character = 'A';
	tid1_args.count = 300;

	tid2_args.character = 'O';
	tid2_args.count = 250;
	pthread_create(&tid1,NULL,char_print,(void *)&tid1_args);
	pthread_create(&tid2,NULL,char_print,(void *)&tid2_args);
	sleep(1);
	return 0;
}