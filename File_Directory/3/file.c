#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include "apue.h" 

/*

    Data una directory da riga di comando, ne elenchi il contenuto e ,individutati i link simbolici,
    indichi se il file puntato ha i diritti di scrittura per il proprietario e il per il gruppo
    
*/
int main (int argc, char *argv[]){
if (argc < 1){
    printf("\nfornire directory all'avvio");
    exit(353);
}
DIR *cartella = opendir(argv[1]);
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
       return 2;
    }
    printf("\nnome: %s: ", elem->d_name);
    if(lstat(elem->d_name, &info)<0){
        perror("\nstat file error");
        continue;
    }
    
    if (S_ISLNK(info.st_mode )){
        printf("LINK SIMBOLICO - ");
        stat(elem->d_name, &info); //per il file puntato dal link simbolico
        
        if((info.st_mode & S_IWUSR) && (info.st_mode & S_IWGRP)){
            printf("PERMESSI DI SCRITTURA USER E GRUPPO OK");
        }
    }
    
   
}
return 4;
}