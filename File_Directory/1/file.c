#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char const *argv[])
{
	DIR *dp;
	struct dirent *dirp;
	char name[1024];
	struct stat buf;


	printf("Inserisci il nome della cartella: ");
	scanf("%s",name);
	if((dp = opendir(name))  == NULL){
		printf("ERRORE opendir\n");
		exit(-1);
	}

	while((dirp = readdir(dp)) != NULL){
		if(lstat(dirp->d_name,&buf)<0){
			printf("errore\n");
		}
			printf("%s\n", dirp->d_name);

		if (S_ISLNK(buf.st_mode)){
			printf("%s\n", dirp->d_name);
		}
	}

	closedir(dp);

	return 0;
}