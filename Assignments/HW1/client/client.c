#include "../includes.h"

#define PORT 0x0da2
#define IP_ADDR 0x7f000001

int download(char **fileName, int sock, int len); // does a pull request from the server for n files 
int upload(char *fileName, int sock, int len); //does a push request to the server for n files

int main(int argc, char *argv[])
{
	int nrecv;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	int len = 0;
	int i = 0;

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	int mode = 0;
	if (strcmp(argv[1], "upload") == 0)
		mode = 1;

	if (connect(sock, (struct sockaddr *)&s, sizeof(s)) < 0)
	{
		perror("Error connecting to the socket");
		return -1;
	}

	printf("Successfully connected.\n");

	int fnum = argc - 2;

	if (send(sock, &fnum, sizeof(int), 0) < 0)
	{
		perror("Error could not send data");
		return -1;
	}

	if (send(sock, &mode, sizeof(int), 0) < 0)
	{
		perror("Error could not send data");
		return -1;
	}

	if (mode == 0)
	{
		if(download(argv, sock, argc-2) == -1)
		{
			int x = 0;
			if (send(sock, &x, sizeof(int), 0) < 0)
			{
				perror("Error occured while downloading a file");
			}
		}

		printf("Finished downloading files from the server\n");
	}
	if (mode == 1)
	{
		for (i = 2; i < argc; ++i)
		{
			len = strlen(argv[i]);
			if(upload(argv[i], sock, len) == -1)
			{
				int x = 0;
				if (send(sock, &x, sizeof(int), 0) < 0)
				{
					perror("Error occured while uploading a file");
				}
			}		
		}

		printf("Finished uploading files to the server\n");
	}

	close(sock);
	return 0;
}

int download(char **fileName, int sock, int len)
{
	int nrecv = 0;
	int mode = 0;
	int i = 0;
	
	char file[256];
	if(len > 1)
		strcpy(file,"Files.zip");
	else
		strcpy(file,fileName[2]);

	if (send(sock, &len, sizeof(int), 0) < 0)
	{
		perror("Error sending data");
		return -1;
	}

	for(i = 0; i < len; ++i)
	{
		int nameLen = strlen(fileName[i+2]);

		printf("%s %d\n\n",fileName[i+2],nameLen);
		if (send(sock, &nameLen, sizeof(int), 0) < 0)
		{
			perror("Error sending data");
			return -1;
		}

		if (send(sock, fileName[i+2], nameLen, 0) < 0)
		{
			perror("Error sending data");
			return -1;
		}
	}

	printf("1\n");

	int check = 0;
	if ((nrecv = recv(sock, &check, sizeof(int), 0)) < 0)
	{
		perror("Error couldnt receive data");
		return 0;
	}

	if (check != 1)
	{
		printf("one of the fiekes is not on the server");
		perror("this file does not exist on the server.");
		return 0;
	}


	printf("1\n");
	off_t size = 0;
	if ((nrecv = recv(sock, &size, sizeof(size), 0)) < 0)
	{
		perror("Error couldnt receive data");
		return 0;
	}

	if(size == 0)
	{
		perror("Didnt receive data from server !");
		return 0;
	}

	void *src = malloc(size * sizeof(void *));
	if(src == NULL)
	{
		perror("Couldnt allocate memory");
		return 0;
	}

	memset(src, 0, size);

	if ((nrecv = recv(sock, src, size, 0)) < 0)
	{
		perror("Error couldnt receive data");
		return 0;
	}

	if(*((int*)src) == 0)
	{
		perror("Didnt receive data from server !");
		return 0;
	}

	printf("1\n");
	int fdout = 0;
	if ((fdout = open(file, O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0)
	{
		perror("can't open");
		return 0;
	}

	if (lseek(fdout, size - 1, SEEK_SET) == -1)
	{
		perror("lseek error");
		return 0;
	}
	if (write(fdout, "", 1) != 1)
	{
		perror("write error");
		return 0;
	}
	printf("1\n");

	void *dst;

	if ((dst = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED)
	{
		perror("mmap error for output");
		return 0;
	}

	memcpy(dst, src, size);

	printf("Files downloaded successfully\n");

	return 0;
}

int upload(char *fileName, int sock, int len)
{
	int fdout = 0;
	struct stat statbuf;
	void *src = 0;
	int mode = 1;
	
	if ((fdout = open(fileName, O_RDONLY)) < 0)
	{
		perror("Error could not open the file");
		return -1;
	}


	if (send(sock, &len, sizeof(int), 0) < 0)
	{
		perror("Error could not send data");
		return -1;
	}

	if (send(sock, fileName, len, 0) < 0)
	{
		perror("Error could not send data");
		return -1;
	}

	if (fstat(fdout, &statbuf) < 0)
	{
		perror("Error could not allocate stats about the file");
		return -1;
	}

	if (send(sock, &statbuf.st_size, sizeof(off_t), 0) < 0)
	{
		perror("Error could not send data");
		return -1;
	}

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdout, 0)) == MAP_FAILED)
	{
		perror("Error could not map to memory");
		return -1;
	}

	if (send(sock, src, statbuf.st_size, 0) < 0)
	{
		perror("Error could not send datar");
		return -1;
	}

	printf("Files uploaded successfully\n");

	return 0;
}