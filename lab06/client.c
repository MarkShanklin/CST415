/***********************************************************
* File Name     : sever.c
* Purpose       : DNS Resolver Client
* Creation Date : 10-21-2017
* Last Modified : Tue 01 Nov 2017 11:43:34 PM PDT
* Created By    : Mark Shanklin 
***********************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "nameserver.h"
#include "tscounter.h"
#include "getport.h"

#define PORT 50050
#define MAX_CONS 5

typedef enum {false,true} bool;
static char serviceName[MAX_SERVICE_NAME_LEN + 1] = "MarkOne";
static int port;

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0, exitVal = 0;
    char recvBuff[MAX_SERVICE_NAME_LEN + 1];
    struct sockaddr_in serv_addr;
	struct addrinfo *addr;
	
	setup_ns(NULL, PORT);
	port = lookup_port(serviceName);
	printf("%d\n", port);
	memset(&serv_addr, '0', sizeof(serv_addr));
	getaddrinfo("unix.cset.oit.edu", NULL, NULL, &addr);
	memcpy(&serv_addr, addr->ai_addr, addr->ai_addrlen);

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
		memset(serviceName, '0', sizeof(serviceName));
		fgets(serviceName,sizeof(serviceName),stdin);
		//write(TCP)
		write(sockfd, serviceName, strlen(serviceName));
		n = 0;
		
		memset(recvBuff, '0',sizeof(recvBuff));
		//while (!EOF) read(TCP)
		while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
			recvBuff[n] = 0;
			if(strcmp(recvBuff, "Shutdown") == 0)
			{
				exitVal = 1;
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