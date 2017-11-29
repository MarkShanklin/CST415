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

static char serviceName[MAX_SERVICE_NAME_LEN + 1] = "MarkOne1";
static int port;
static bool verbose = false;
static char IP[16];
static services_t *serviceCache;
static services_t *database;

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
	services_t* temp = serviceCache;
	services_t* newNode = GetNewNode(ip,name);
	if(serviceCache == NULL) {
		serviceCache = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

static void Print(int connfd) {
	services_t* temp = serviceCache;
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

static int getDNS_Data(char *message, int connfd)
{
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	char dnsdata[65536];
	char retMsg[65536];
	char temp[256];
	char strIP[16];
	char convMess[strlen(message)+2];
	char tokenTemp[2];
	int len = 0;
	int offset = 0;
	memset(dnsdata, 0, sizeof(dnsdata));
	memset(convMess, 0, sizeof(convMess));
	memset(temp, 0, sizeof(temp));

	struct sockaddr_in place;

	place.sin_family = AF_INET;
	place.sin_port = htons(53);
	place.sin_addr.s_addr = inet_addr("8.8.8.8"); //need to be in the database

 	dnsHeader_t *header;
	dnsRecord_t * recdata;
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
		sprintf(tokenTemp,"%c",(char)len);
		strcat(convMess, tokenTemp);
		strcat(convMess, token);
		token = strtok(NULL, ".");
	}
	
/* 	for(int i = 0; i < strlen(convMess); i++)
	{ 
		dnsdata[sizeof(dnsHeader_t)+ i] = convMess[i];
		if((int)convMess[i] < 60)
		{
			printf("%d", (int)convMess[i]);
		}
		else
		{
			printf("%c", convMess[i]);
		}
	}
	printf("\n"); */ 

	question = (dnsQuestion_t*)&dnsdata[sizeof(dnsHeader_t)+strlen(convMess)+1];
	question->qt = htons(1);
	question->qc = htons(1);

	sendto(fd, (char*)dnsdata, sizeof(dnsHeader_t)+
	strlen(convMess)+1+sizeof(dnsQuestion_t), 0, 
	(struct sockaddr*)&place, sizeof(place));

	memset(dnsdata, 0, sizeof(dnsdata));

	int leng = sizeof(place);
	recvfrom(fd, (char*)dnsdata, sizeof(dnsdata), 0, 
	(struct sockaddr*)&place,(socklen_t*)&leng);

	header = (dnsHeader_t*)dnsdata;

	printf("Responce header:\n\n");
	printf("Questions:\n %d\n", ntohs(header->qd));
	printf("Answers:\n %d\n", ntohs(header->an));
	printf("Authoritative Servers:\n %d\n", ntohs(header->ns));
	printf("Additional Records:\n %d\n\n", ntohs(header->ar));

	char* rName;
	char* rData; 
	offset = 
	(sizeof(dnsHeader_t) + strlen(convMess) + 1 + sizeof(dnsQuestion_t));
	rName = &dnsdata[offset];

	if((uint8_t)(rName[0]) >= 192)
	{
		recdata = (dnsRecord_t*)&dnsdata[offset + 2];
		rData = (char*)&dnsdata[offset + sizeof(dnsRecord_t)];
		offset = (uint8_t)rName[1];
	}else
	{
		recdata = (dnsRecord_t*)&dnsdata[offset + strlen((char*)rName)]; 
		rData =
		(char*)&dnsdata[offset + sizeof(dnsRecord_t) + strlen((char*)rName)];
	}
	memset(temp,0,sizeof(temp));

	printf("Offset data - %d:\n", offset);

	for(int i = 0; dnsdata[i + offset] != 0; i++)
	{
		temp[i] = dnsdata[i + offset];
	}

	translate((char*)temp);
	sprintf(message, "%s", (char*)temp);

	printf("Name:\t%s\n", temp);
	printf("Type:\t%d\n", ntohs(recdata->tp));
	printf("Class:\t%d\n", ntohs(recdata->cl));
	printf("TTL:\t%d\n", ntohs(recdata->tl));
	printf("Len:\t%d\n", ntohs(recdata->rl));
	printf("Data:\t");

	if(ntohs(recdata->tp) == 1)
	{
		sprintf((char*)temp, "%d", (uint8_t)rData[0]);
		strcat((char*)strIP, (char*)temp);
		if (verbose == true)
		{
			strcat(retMsg, "\n");
			strcat(retMsg, (char*)temp);
		}
		else
		{
			sprintf(retMsg, "%s", (char*)temp);
		}

		for(int i = 1; i < ntohs(recdata->rl); i++)
		{
			sprintf((char*)temp, ".%d", (uint8_t)rData[i]);
			strcat(retMsg, (char*)temp);
			strcat((char*)strIP, (char*)temp);
		}
		push(strIP,message);
	}
	else if (ntohs(recdata->tp) == 5)
	{
		int tempOffset = 
		sizeof(dnsHeader_t) + sizeof(dnsQuestion_t) +
		strlen(convMess) + 2 + sizeof(dnsRecord_t);

		char tempHolder[256];
		memset(tempHolder,0, 256);
		bool done = false;
		while(done != true)
		{
			if((uint8_t)dnsdata[tempOffset] == 0)
			{}
			else if((uint8_t)dnsdata[tempOffset] >= 192)
			{
				tempOffset = (uint8_t)dnsdata[tempOffset + 1];
			}
			else
			{
				int count = (uint8_t)dnsdata[tempOffset]+1;
				for(int i = 0; i < count; i++)
				{
					if ((uint8_t)dnsdata[tempOffset+i] == 0)
					{
						done = true;
					}
					else
					{
						memset(tokenTemp, 0, 2);
						if(dnsdata[tempOffset + i] < 60)
						{
							sprintf(tokenTemp, "%d", (uint8_t)dnsdata[tempOffset + i]);
						} 
						else 
						{
							sprintf(tokenTemp, "%c", dnsdata[tempOffset + i]);
						}
							strcat((char*)tempHolder, tokenTemp);
					}	
				}
			}
		}

		//recurse(tempHolder, tempOffset, dnsdata);
		if(verbose == true)
		{
			strcat(retMsg, "\n");
			strcat(retMsg, tempHolder);
		}
		else
		{
			sprintf(retMsg, "%s", tempHolder);
		}
	}
	write(connfd,retMsg, sizeof(retMsg));
	close(connfd);
	//if DNS does not reply soon enough
						//try again (only once)
					//while DNS data is another DNS
						//ask new DNS for data
						//add new DNS to list of DNS's
	//reply with IP address or "failure"
	return EXIT_SUCCESS;
}

int translate(char* msg)
{
	char translated[256];
	memset(translated, 0, 256);
	int count = 0;
	for(int i = 0; msg[i] != 0; i++)
	{
		count = (uint8_t)msg[i];
		for(int j = 0; j < count; j++, i++)
		{
			translated[i] = msg[i + 1];
		}
		translated[i] = '.';
		count = i;
	}
	translated[count] = 0;
	memset(msg, 0, 256);
	sprintf(msg,"%s", translated);
	return 0;
}

static void* runThread(void * data)
{
	conn_t * temp = (conn_t*)data;
	
	//assume text is to be resolved
	//check cache data struct 
	services_t * travel = serviceCache;
	while(travel != NULL && strcmp(travel->serviceName, temp->message) != 0)
	{
							travel = travel->next;
	}
	if(travel != NULL)
	{
		//reply with IP address
		write(temp->connfd, travel->serviceIP, 16);
		close(temp->connfd);
	}
	else
	{
		//if data not in cache
		//ask closest DNS for data
		getDNS_Data(temp->message, temp->connfd);
		//send reply data to client
		//write(temp->connfd, temp->message, strlen(temp->message));
	}
	//close connection
	//close(temp->connfd);
	//close thread
	//pthread_exit(EXIT_SUCCESS);
	return temp;
}

int main(int argc, char *argv[])
{	
	//used to handle comand line argurements.
	int command = 0;
	 if(argc < 3)
	 {
		printf("-n \t<nameserver> is required.\n");
		return EXIT_FAILURE;
	 }
	//command line parsing.
    while ((command = getopt(argc, argv, "n:h")) != -1) 
    {
        switch (command)
        {
        case 'n':
        	strcpy(IP, optarg);
            break;
        case 'h':
            printf("This program is a DNS resolver.\n\n"
                   "-n \t<nameserver>\n");
            break;
        case '?':
            break;
        }
    }
	//translate "-n nameserver" (getaddrinfo)
	struct addrinfo *addr;	
	getaddrinfo(IP,NULL,NULL, &addr);
	push((char*)addr->ai_addr->sa_data, &IP);
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
			memset(message, 0, sizeof(message));
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
					close(connfd);
				}
				else if(strcmp("verbose", message) == 0)
				{
					//toggle verbose flag to true, reply successful
					verbose = true;
					write(connfd,"SUCCESS", strlen("SUCCESS"));
					close(connfd);
				}
				else if(strcmp("normal", message) == 0)	
				{
					
					//toggle verbose flag to false, reply successful
					verbose = false;
					write(connfd,"SUCCESS", strlen("SUCCESS"));
					close(connfd);
				}
				else if(strcmp("shutdown", message) == 0)
				{
					//toggle shutdown flag to true 
					tsc_reset();
					write(connfd,"shutdown", strlen("shutdown"));
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
					runThread(&data);
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