#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc,const char *argv[])
{
    int fdin;
    void * src;
    struct stat statbuf;

    if ((fdin= open(argv[1], O_RDONLY)) < 0)
        return 0;
    if (fstat(fdin, &statbuf) < 0)
        return 0;
    if ((src= mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,fdin, 0)) == MAP_FAILED)
        return 0;

    return src;
}