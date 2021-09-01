#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>


/*
	Stampa in output il nome del link simbolico presente nella directory corrente che si riferisce al file regolare
	di taglia più grande.
    
    
*/
int main (int argc, char *argv[]){

int max=-1;
int namelen;
char nameFile[1024],buf[1024];
char nameDirectory[1024];
getcwd(nameDirectory,sizeof(nameDirectory));
DIR *cartella = opendir(nameDirectory);
if (cartella == NULL){
    perror("\nerrore apertura cartella ");
    exit(542);
}
chdir(argv[1]);
struct dirent *elem;
struct stat info;
while (1){
    elem=readdir(cartella);
    if(elem==NULL){
       break;
    }
    if(lstat(elem->d_name, &info)<0){
        perror("\nstat file error");
        continue;
    }
    
    if (S_ISLNK(info.st_mode )){
        stat(elem->d_name, &info); //per il file puntato dal link simbolico
        printf("DIMENSIONE FILE puntato da %s: %d ",elem->d_name,(int)info.st_size);

        if ((namelen = readlink(elem->d_name, buf, sizeof(buf))) == -1){
	  		 perror("readlink");
	   		return 3;
		}

       buf[namelen] = '\0';
       printf("link to: %s\n", buf);
     
       if(info.st_size > max){
       	max = info.st_size;
       	strcpy(nameFile,elem->d_name);
       }
    }
    
   
}

printf("\nname link simbolico **%s** max %d\n",nameFile, max);
return 4;
}