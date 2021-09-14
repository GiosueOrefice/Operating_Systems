#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>


/*
	Leggere i file della directory corrente, se il file letto è un directory mostrare il contenuto 
	della directory (Non mostrare i contenuti di eventuali sotto-directory)  e creare un processo figlio 
	che esegue il comando ls -l sulla directory.	

*/

int main(int argc, char const *argv[])
{
	
	struct dirent *dirent,*dirent2;
	struct stat info,info2;
	char nameDirectory[1024];
	pid_t pid;

	getcwd(nameDirectory,1024);
	DIR *dir = opendir(nameDirectory);
	if(dir == NULL){
		printf("opendir error\n");
		exit(-1);
	}

	while((dirent = readdir(dir))!=NULL){
		lstat(dirent->d_name,&info);
		if(info.st_mode & S_IFDIR && strncmp(dirent->d_name,"..",2) && strncmp(dirent->d_name,".",1)){
			printf("%s e' una directory \n\n",dirent->d_name);

				DIR *dir2 = opendir(dirent->d_name);
				if(dir2 == NULL){
					printf("opendir2 error\n");
					exit(-4);
				}
				

				if((pid= fork())<0){
					perror("fork");
					exit(-2);
				}

				
				if(pid == 0){
					printf("FIGLIO - STAMPA DEL CONTENUTO DI %s\n",dirent->d_name);
					execlp("ls","ls","-l",dirent->d_name,NULL);
					perror("execlp");
					exit(-5);
					
				}

				else {
					waitpid(pid,NULL,0);
					printf("\n");
					while((dirent2 = readdir(dir2))!=NULL){
						printf("%s   ",dirent2->d_name);

					}
					printf("\n\n");
					
				}
				
		}
	}
	
	return 0;
}

