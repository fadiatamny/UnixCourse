#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc,const char *argv[])
{
    int fdout;
    void * dist;
    struct stat statbuf;

    if ((fdout= open(argv[2], O_RDWR | O_CREAT | O_TRUNC,S_IRUSR|S_IWUSR)) < 0)
        return 0;
}