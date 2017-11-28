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
static char serviceName[MAX_SERVICE_NAME_LEN + 1] = "MarkOne1";
static int port;

typedef struct node{
	char serviceIP[16];
	char serviceName[MAX_SERVICE_NAME_LEN + 1];
	struct node *next;
	struct node *prev;
} services_t;

typedef struct {
	int connfd;
	char message[MAX_SERVICE_NAME_LEN + 1];
} conn_t;

static services_t *serv;

static services_t* GetNewNode(char * ip, char * name)
{
	services_t* newNode
		= (services_t*)malloc(sizeof(services_t));
	strcpy(newNode->serviceIP,ip);
	strcpy(newNode->serviceName, name);
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}	

static void push(char * ip, char * name) {
	services_t* temp = serv;
	services_t* newNode = GetNewNode(ip,name);
	if(serv == NULL) {
		serv = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

static void Print(int connfd) {
	services_t* temp = serv;
	printf("cache contents:\n");
	write(connfd, "cache contents:\n", strlen("cache contents:\n"));
	while(temp != NULL) {
		printf("%s-%s\n",temp->serviceName, temp->serviceIP);
		write(connfd, temp->serviceName, strlen(temp->serviceName));
		write(connfd, "-", strlen("-"));
		write(connfd, temp->serviceIP, strlen(temp->serviceIP));
		write(connfd, "\n", strlen("\n"));
		temp = temp->next;
	}
	//database now
}

typedef struct {
	uint16_t id;
	unsigned char rd : 1, tc : 1, aa : 1, op : 3, qr : 1;
	unsigned char rc : 4, zz : 3, ra : 1;
	uint16_t qd, an, ns, ar;	
} dnsHeader_t;

typedef struct {
	uint16_t tp, cl;
	uint32_t tl;
	uint16_t rl;
} dnsRecord_t;

typedef struct {
	uint16_t qt, qc;
} dnsQuestion_t;

static int getDNS_Data(char *message, int connfd)
{
	char dnsdata[65536];
	char convMess[strlen(message)+2];
	char temp[2];
	int len = 0;
	memset(dnsdata, 0, sizeof(dnsdata));
	memset(convMess, 0, sizeof(convMess));
	memset(temp, 0, sizeof(temp));

 	dnsHeader_t *header;
	dnsRecord_t *record;
	dnsQuestion_t *question;
	
	header = (dnsHeader_t*)&dnsdata[0];
	header->id = htons((uint16_t)connfd); //change to unique
	header->rd = 1;
	header->qd = htons(1);
	
	char* token;// = &dnsdata[sizeof(dnsHeader_t)];
	
	token = strtok(message, ".");
	
	while(token != NULL)
	{
		len = strlen(token);		
		sprintf(temp,"%c",(char)len);
		strcat(convMess, temp);
		strcat(convMess, token);
		token = strtok(NULL, ".");
	}
	
	
	for(int i = 0; i < strlen(convMess); i++)
	{
		if((int)convMess[i] < 60)
		{
			printf("%d", (int)convMess[i]);
		}
		else
		{
			printf("%c", convMess[i]);
		}
	}
	printf("\n"); 
	write(connfd,convMess, sizeof(convMess));
	close(connfd);
	//if DNS does not reply soon enough
						//try again (only once)
					//while DNS data is another DNS
						//ask new DNS for data
						//add new DNS to list of DNS's
	//reply with IP address or "failure"
	return EXIT_SUCCESS;
}

static void* runThread(void * data)
{
	conn_t * temp = (conn_t*)data;
	
	//assume text is to be resolved
	//check cache data struct 
	services_t * travel = serv;
	while(travel != NULL && strcmp(travel->serviceName, temp->message) != 0)
	{
							travel = travel->next;
	}
	if(travel != NULL)
	{
		//reply with IP address
		write(temp->connfd, travel->serviceIP, 16);
	}
	else
	{
		//if data not in cache
		//ask closest DNS for data
		getDNS_Data(temp->message, temp->connfd);
		//send reply data to client
		write(temp->connfd, temp->message, strlen(temp->message));
	}
	//close connection
	close(temp->connfd);
	//close thread
	//pthread_exit(EXIT_SUCCESS);
	return temp;
}

int main(int argc, char *argv[])
{	//translate "-n nameserver" (getaddrinfo)
	struct addrinfo *addr;	
	getaddrinfo("8.8.8.8",NULL,NULL, &addr);
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
    int bound = sock2port(listenfd);
    printf("\nBound to port: %d\n", bound);

    char message[1024];
    memset(message, 0, sizeof(message)); 
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
            int connfd = 
			accept(listenfd, (struct sockaddr*)&recvAddr, &recvLen);
			//recieve request
            numRead= read(connfd, &message, sizeof(message));
			//create new thread
			if(numRead > -1)
            {
				printf("message: %s\n", message);
				if (strcmp("exit", message) == 0)
				{
					//exit
					write(connfd,"SUCCESS", strlen("SUCCESS"));
					close(connfd);
					tsc_decrement();
				}
				else if(strcmp("dump", message) == 0)
				{
					Print(connfd);
					write(connfd,"SUCCESS", strlen("SUCCESS"));
				}
				else if(strcmp("verbose", message) == 0)
				{
					//toggle verbose flag to true, reply successful
					write(connfd,"SUCCESS", strlen("SUCCESS"));
				}
				else if(strcmp("normal", message) == 0)	
				{
					//toggle verbose flag to false, reply successful
					write(connfd,"SUCCESS", strlen("SUCCESS"));
				}
				else if(strcmp("shutdown", message) == 0)
				{
					//toggle shutdown flag to true 
					tsc_reset();
					close(connfd);
					Run = false;
					//reply successful
				}
				else
				{
					conn_t data;
					data.connfd = connfd;
					strcpy(data.message, message);
					//pthread_create(*thread, *attr, &runThread, connfd);
					//runThread(&data);
					getDNS_Data(message, connfd);
				}
			}
            else
            {
                fprintf(stderr,"ERROR");
            }
        }
    }
	//clean up data structures
    release_port(serviceName, port); 
    return 0;
}