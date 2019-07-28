#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int isDirectory(const char* path)
{
    struct stat pStat;
    stat(path,&pStat);
    return S_ISDIR(pStat.st_mode);
}

void readWriteBuff(int r,int w)
{
    ssize_t n;
    char buff[256];
    while((n = read(r,buff,256))>0)
    {
        printf("%s\n",buff);
        if(write(w,buff,n) != n)
            return;
    }

    close(r);
    close(w);
}

int main (int argc, const char* argv[]) 
{
    if(argc < 3)
    {
        printf("error not enough files");
        exit(-1);
    }

    char buff[256] = {0};
    char in[256] = {0};
    char out[256] = {0};

    if(strcmp(argv[1],argv[2])==0)
    {
        printf("same file!");
        exit(-1);
    }

    strcpy(in,argv[1]);

    if(isDirectory(argv[2]))
    {
        char path[256] = {0};
        strcat(path,argv[2]);
        path[strlen(path)]='/';
        strcat(path,in);
        strcpy(out,path);
    }
    else
    {
        strcpy(out,argv[2]);
    }

    int in_fd = open(in,O_RDONLY);
    int out_fd = open(out,O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    if (in_fd < 0 || out_fd < 0)
    {
        perror("error accesing files");
        exit(-1);
    }

    readWriteBuff(in_fd,out_fd);
};