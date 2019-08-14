#include <stdio.h> 
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



static int my_ls (char* path)
{
    DIR* directory; 
    struct dirent* ent; 
    char buf[256]; 
    int flag = 0;

    if ((directory = opendir(path)) == NULL) {
        getcwd(&buf,256);
        if ((directory = opendir(&buf)) == NULL) {
             //error exit
         }
        flag = 1;
    }
    while ((ent = readdir(directory)) != NULL) printf("%s\n", ent->d_name);
     
return 0; 
}

static int my_cd (char* path){
    int re = chdir(path);
    return re;
}

static int my_system (char* command){
    int re = system(command);
    return re;
}



int main (int argc , const char* argv[])
{

char first[256];
char secnd[256];
    while (1)
    {
        printf(">");
        scanf ("%s %s",first,secnd);
        if (strncmp(first,"ls",2)== 0){
            my_ls(secnd);
        }

        if (strncmp(first, "cd",2)==0){
            my_cd(secnd);
        }

        else
        {
            my_system(first);
        }
    }
}
    