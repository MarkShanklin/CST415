/***********************************************************
* File Name     : sever.c
* Purpose       : DNS Resolver Server
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

typedef struct {
	char serviceIP[];
	char serviceName[MAX_SERVICE_NAME_LEN + 1];
} services_t;

static services_t serv[256];

int main(int argc, char *argv[])
{	//translate "-n nameserver" (getaddrinfo)
	
    //initialization the name server
    setup_ns(NULL, PORT);
    tsc_reset();

    //connect to the name server
    port = request_port(serviceName);

    struct sockaddr_in serverAddr;
    struct sockaddr_in recvAddr ;

	//create and bind socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*)&serverAddr, '0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//unix.cset.oit.edu
    serverAddr.sin_port = htons(port);
    bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    int bound = sock2port(fd);
    printf("\nBound to port: %d", bound);

    char message[1024];
    memset(message, '0', sizeof(message)); 
    socklen_t recvLen = sizeof(recvAddr);
    int numRead;
    bool Run = true;
	
	//listen for incoming connections
    listen(listenfd, 10);
	//loop until "shutdown" recieved
    while(Run)
    {
        if(tsc_value() < MAX_CONS)
        {
            int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
			//recieve request
            numRead= read(connfd, &message, sizeof(message));
			//create new thread
            if(numRead > -1)
            {
                if (strcmp("exit", message) == 0)
                {
                    //exit
                    release_port();
                    tsc_decrement();
                }
				else if(strcmp("dump", message) == 0)
				{
					//dump database contents to client (human readable)
				}
				else if(strcmp("verbose", message) == 0)
				{
					//toggle verbose flag to true, reply successful
				}
				else if(strcmp("normal", message) == 0)	
				{
					//toggle verbose flag to false, reply successful
				}
                else if(strcmp("shutdown", message) == 0)
                {
                    //toggle shutdown flag to true 
					tsc_reset();
                    Run = false;
					//reply successful
                }
                else
                {
				//assume text is to be resolved
				//check cache data struct 
				for(int i = 0; i < 256; i++)
				{
					strcmp(serv[i].serviceIP, message);
				}
				//if data not in cache
					//ask closest DNS for data
					//if DNS does not reply soon enough
						//try again (only once)
					//while DNS data is another DNS
						//ask new DNS for data
						//add new DNS to list of DNS's
                }
				//reply with IP address or "failure"
				//send reply data to client
				//while not done sending data
					//send more reply data to client
            }
            else
            {
                fprintf(ftderr,"ERROR");
            }
			//close thread
			//close connection
            close(connfd);
        }
    }
	//clean up data structures
    release_port(serviceName, port);
    return 0;
}