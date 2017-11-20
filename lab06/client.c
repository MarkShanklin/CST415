#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <pthread.h>

#include "nameserver.h"
#include "getport.h"
#include "tscounter.h"
#include "addr2str.h"

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0, exitVal = 0, port;
    char recvBuff[1024];
	char buff[256];
    struct sockaddr_in serv_addr;
	struct addrinfo *addr;
	
	setup_ns(NULL, 50050);
	port = lookup_port("MarkOne");
	
	memset(&serv_addr, '0', sizeof(serv_addr));
	getaddrinfo("unix.cset.oit.edu", NULL, NULL, &addr);
	
	memccpy(&serv_addr, addr->ai_addr, addr->ai_addrlen);

	while( exitVal == 0)
	{		
		//form TCP connection
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			fprintf(stderr,"\n ERROR : COULDN'T CREATE SOCKET \n");
			return EXIT_FAILURE;
		}
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port); 

		if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
		   fprintf(stderr,"\n ERROR : CONNECT FAILED \n");
		   return EXIT_FAILURE;
		}
		//fgets(stdin)
		fprintf(stdout, "Enter URL: ");
		memset(buff, '0', sizeof(buff));
		fgets(buff,sizeof(buff),stdin);
		//write(TCP)
		write(sockfd, buff, strlen(buff));
		n = 0;
		
		memset(recvBuff, '0',sizeof(recvBuff));
		//while (!EOF) read(TCP)
		while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
			recvBuff[n] = 0;
			if(strcmp(recvBuff, "Shutdown") == 0)
			{
				//shutdown;
			}
			//write(stdout)
			if(fputs(recvBuff, stdout) == EOF)
			{
				fprintf(stderr,"\n ERROR : FPUTS ERROR\n");
			}
		} 
		if(n < 0)
		{
			fprintf(stderr,"\n ERROR : READ ERROR \n");
		}
		close(sockfd);
	}
    return EXIT_SUCCESS;
}