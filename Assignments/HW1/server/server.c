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
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define QUEUE_LEN 20

int download(int newfd, int fNum); //receives socket and number of files and uploads files from client to directory.
int upload(int newfd, int fNum); //receives socket and number of files and sends files to client.

int main(int argc, char *argv[])
{
	int listenS = socket(AF_INET, SOCK_STREAM, 0);
	if (listenS < 0)
	{
		perror("Error creating socket");
		return -1;
	}
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);

	if (bind(listenS, (struct sockaddr *)&s, sizeof(s)) < 0)
	{
		perror("Error could not bind to socket");
		return -1;
	}
	if (listen(listenS, QUEUE_LEN) < 0)
	{
		perror("Error could not listen to socket");
		return -1;
	}

	while (1)
	{
		struct stat statbuf;
		struct sockaddr_in clientIn;
		int clientInSize = sizeof clientIn;
		int mode = 2;
		int newfd = accept(listenS, (struct sockaddr *)&clientIn, (socklen_t *)&clientInSize);
		int fNum = 0;

		if (newfd < 0)
		{
			perror("Error could not accept connection");
			return -1;
		}

		int pid = fork();

		if (pid == 0)
		{
			if (recv(newfd, &fNum, sizeof(int), 0) < 0)
			{
				perror("recv");
				return -1;
			}

			if (recv(newfd, &mode, sizeof(int), 0) < 0)
			{
				perror("recv");
				return -1;
			}

			if (mode == 0)
			{
				printf("Received Request to Download %d files\n", fNum);

				if (download(newfd, fNum) == -1)
				{
					int x = 0;
					if (send(newfd, &x, sizeof(int), 0) < 0)
					{
						perror("couldnt send failure message");
					}
				}
				else
					printf("Done sending data to client %d. Closing socket.\n\n", newfd);

				close(newfd);
				exit(1);
			}

			if (mode == 1)
			{
				printf("Received Request to Upload %d files\n", fNum);

				if (upload(newfd, fNum) == -1)
				{
					int x = 0;
					if (send(newfd, &x, sizeof(int), 0) < 0)
					{
						perror("couldnt send failure message");
					}
				}
				else
					printf("Done receiving data to client %d. Closing socket.\n\n", newfd);

				close(newfd);
				exit(1);
			}
			else
			{
				perror("Invalid mode received");
				exit(1);
			}
		}
	}
	return 0;
}

int download(int newfd, int fNum)
{
	int len = 0;
	int fdin = 0;
	struct stat statbuf;
	void *src;
	int i = 0;

	for (i = 0; i < fNum; ++i)
	{
		if (recv(newfd, &len, sizeof(int), 0) <= 0)
		{
			perror("Error couldnt receive data");
			return 0;
		}

		if(len == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		char *buff = (char *)malloc((len + 1) * sizeof(char));
		
		if( buff == NULL)
		{
			perror("could not allocate memory");
			return 0;
		}

		memset(buff, 0, len);

		if (recv(newfd, buff, len, 0) <= 0)
		{
			perror("Error couldnt receive data");
			return 0;
		}

		if(buff[0] == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		buff[len] = '\0';
		int check = 1;

		if ((fdin = open(buff, O_RDONLY)) < 0)
		{
			perror("Could not open FILE with given name");
			check = -1;
			if (send(newfd, &check, sizeof(int), 0) < 0)
			{
				perror("Error could not send data");
				return -1;
			}
			return -1;
		}

		if (send(newfd, &check, sizeof(int), 0) < 0)
		{
			perror("Error could not send data");
			return -1;
		}

		if (fstat(fdin, &statbuf) < 0)
		{
			perror("Error could not allocate stats about the file");
			return -1;
		}

		if (send(newfd, &statbuf.st_size, sizeof(off_t), 0) < 0)
		{
			perror("Error could not send data");
			return -1;
		}

		if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED)
		{
			perror("Error could not map to memory");
			return -1;
		}

		if (send(newfd, src, statbuf.st_size, 0) < 0)
		{
			perror("Error could not send data");
			return -1;
		}

		printf("Sent %s to client",buff);

		free(buff);
	}

	return 0;
}

int upload(int newfd, int fNum)
{
	int i = 0;
	int len = 0;
	int fdin = 0;
	int nrecv = 0;
	struct stat statbuf;

	for (i = 0; i < fNum; ++i)
	{
		if (recv(newfd, &len, sizeof(int), 0) <= 0)
		{
			perror("Error could not receive data");
			return 0;
		}
		
		if(len == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		char *buff = (char *)malloc((len + 1) * sizeof(char));

		if( buff == NULL)
		{
			perror("Error could not allocate memory");
			return 0;
		}

		memset(buff, 0, len);

		if (recv(newfd, buff, len, 0) <= 0)
		{
			perror("Error could not receive data");
			return 0;
		}

		if(buff[0] == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		buff[len] = '\0';
		printf("buff = %s\n", buff);

		if ((fdin = open(buff, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
		{
			perror("Error could not open the file");
			return 0;
		}

		off_t size = 0;
		if ((nrecv = recv(newfd, &size, sizeof(size), 0)) <= 0)
		{
			perror("Error could not receive data");
			return 0;
		}

		if(size == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		void *src = malloc(size * sizeof(void *));

		if(src == NULL)
		{
			perror("Error could not allocate memory");
			return 0;
		}

		memset(src, 0, size);

		//recive mmap
		if ((nrecv = recv(newfd, src, size, 0)) <= 0)
		{
			perror("Error could not receive data");
			return 0;
		}

		if(*((int*)src) == 0)
		{
			perror("Didnt receive data from client !");
			return 0;
		}

		if (lseek(fdin, size - 1, SEEK_SET) == -1)
		{
			perror("Error occured with file setup");
			return 0;
		}

		if (write(fdin, "", 1) != 1)
		{
			perror("Error occured with file setup");
			return 0;
		}

		void *dst;

		if ((dst = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdin, 0)) == MAP_FAILED)
		{
			perror("Error could not map to memory");
			return 0;
		}

		memcpy(dst, src, size);

		printf("Uploaded file %s",buff);

		free(buff);
	}

	return 0;
}