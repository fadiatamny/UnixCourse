#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PORT 0x0da2
#define IP_ADDR 0x7f000001

int main(int argc, const char* argv[])
{
	int sock = socket(AF_INET, SOCK_STREAM, 0), nrecv;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		perror("connect");
		return 1;
	}
	
	printf("Successfully connected.\n");
	int flen = strlen(argv[1]);
	if (send(sock, &flen, sizeof(int), 0) < 0)
	{
		perror("send");
		return 1;
	}
	int r = 0;
	if ((nrecv = recv(sock, &r, sizeof(int), 0)) < 0)
	{
		perror("recv");
		return 1;
	}

	if (send(sock, argv[1], flen, 0) < 0)
	{
		perror("send");
		return 1;
	}
	off_t size = 0;

	if ((nrecv = recv(sock, &size, sizeof(off_t), 0)) < 0)
	{
		perror("recv");
		return 1;
	}

	if (send(sock, "recieved", sizeof("recieved"), 0) < 0)
	{
		perror("send");
		return 1;
	}

	printf("%d\n\n",size);

	void * src = calloc(sizeof(void*),size);
	if ((nrecv = recv(sock, src, size, 0)) < 0)
	{
		perror("recv");
		return 1;
	}
	
	int fdout;
    void * dist;
	
    if ((fdout= open(argv[1], O_RDWR | O_CREAT | O_TRUNC,S_IRUSR|S_IWUSR)) < 0)
        return 0;
    if (lseek(fdout, size-1, SEEK_SET) == -1)
        exit(-1);   
    if (write(fdout, "", 1) != 1)
		exit(-1);
    if((dist= mmap(0, size-1, PROT_READ | PROT_WRITE,MAP_SHARED, fdout, 0)) == MAP_FAILED)
		exit(-1);
		
    memcpy(dist, src, size);
    exit(0);
	return 0;
}
