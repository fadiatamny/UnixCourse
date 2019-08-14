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
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

//implement file name dynamically

#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define QUEUE_LEN 20

int main(void)
{
	int listenS = socket(AF_INET, SOCK_STREAM, 0);
	if (listenS < 0)
	{
		perror("socket");
		return 1;
	}
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	if (bind(listenS, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		perror("bind");
		return 1;
	}
	if (listen(listenS, QUEUE_LEN) < 0)
	{
		perror("listen");
		return 1;
	}
	struct sockaddr_in clientIn;
	int clientInSize = sizeof clientIn;
	int newfd = accept(listenS, (struct sockaddr*)&clientIn, (socklen_t*)&clientInSize);
	if (newfd < 0)
	{
		perror("accept");
		return 1;
	}

	printf("Sending data to client %d\n", newfd);
	int leng = 0;
	if(recv(newfd, &leng, sizeof(int), 0) < 0)
	{
		perror("read");
		return 1;
	}
	int tmp = 1;
	if (send(newfd, &tmp, sizeof(int), 0) < 0)
	{
		perror("send");
		return 1;
	}
	printf("the leng = %d\n",leng);

	char * name = (char*) malloc((leng+1) * sizeof(char));
	memset(name,0,leng+1);

	printf("%s test\n",name);

	if(recv(newfd, name, leng, 0) < 0)
	{
		perror("read");
		return 1;
	}

	printf("%s\n",name);

	char buff2[256] = {0};

	int fdin;
    void * src;
    struct stat statbuf;

    if ((fdin= open(name, O_RDONLY)) < 0)
        return 0;
    if (fstat(fdin, &statbuf) < 0)
        return 0;
    if ((src= mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,fdin, 0)) == MAP_FAILED)
        return 0;

	if (send(newfd, &statbuf.st_size, sizeof(statbuf.st_size), 0) < 0)
	{
		perror("send");
		return 1;
	}

	if(recv(newfd, buff2, sizeof(buff2), 0) < 0)
	{
		perror("read");
		return 1;
	}

	printf("%s\n",buff2);


	if (send(newfd, src, statbuf.st_size, 0) < 0)
	{
		perror("send");
		return 1;
	}

	printf("Done sending data to client %d. Closing socket.\n", newfd);
	close(newfd);
	close(listenS);
	return 0;
}
