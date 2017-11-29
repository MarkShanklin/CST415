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
//static services_t *database;

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

int Translate(char *msg)
{
    char Translated[256];
    memset(Translated, 0, 256);
    int count = 0;
    for (int i = 0; msg[i] != 0; i++)
    {
        count = (uint8_t)msg[i];
        for (int j = 0; j < count; j++, i++)
            Translated[i] = msg[i + 1];
        Translated[i] = '.';
        count = i;
    }
    Translated[count] = 0;
    memset(msg, 0, 256);
    sprintf(msg, "%s", Translated);
    return 0;
}

int recursiveTranslate(int offset, char *msg, char *buff)
{
    if((uint8_t)buff[offset] == 0)
    {}
    else if((uint8_t)buff[offset] >= 192)
    {
        offset = (uint8_t)buff[offset + 1];
        recursiveTranslate(offset, msg, buff);
    }
    else
    {
        int count = (uint8_t)buff[offset]+1;
        for(int i = 0; i < count; i++)
        {
            if ((uint8_t)buff[offset+i] == 0)
                return 0;
            char tmp[2];
            memset(tmp, 0, 2);
            if(buff[offset + i] < 60)
            {
                sprintf(tmp, "%d", (uint8_t)buff[offset + i]);
            } else sprintf(tmp, "%c", buff[offset + i]);
            strcat((char*)msg, tmp);
        }
        recursiveTranslate((offset+count), msg, buff);
    }
    return 0;
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
	char messageCopy[sizeof(message)];
	strcpy(messageCopy, message);
	token = strtok(messageCopy, ".");
	
	while(token != NULL)
	{
		len = strlen(token);		
		sprintf(tokenTemp,"%c",(char)len);
		strcat(convMess, tokenTemp);
		strcat(convMess, token);
		token = strtok(NULL, ".");
	}
	fprintf(stderr, "convMess: %s\n", convMess);
	
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

	char* rName;
	char* rData; 
	offset = 
	(sizeof(dnsHeader_t) + strlen(convMess) + sizeof(dnsQuestion_t) + 1);
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
	
	for(int i = 0; dnsdata[i + offset] != 0; i++)
	{
		fprintf(stderr,"dnsdata: %c\n", dnsdata[i + offset]);
		temp[i] = dnsdata[i + offset];
	}
	fprintf(stderr,"temp: %s\n", temp);
	Translate((char*)temp);
	sprintf(convMess, "%s", (char*)temp);

	fprintf(stderr,"message: %s\n", message);
	fprintf(stderr, "convMess: %s\n", convMess);
	fprintf(stderr,"temp: %s\n", temp);

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
		recursiveTranslate(tempOffset, tempHolder, dnsdata);
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

static void* runThread(char* message, int connfd)
{
	//conn_t * temp = (conn_t*)data;
	
	//assume text is to be resolved
	//check cache data struct 
	services_t * travel = serviceCache;
	bool found = false;
	while(travel != NULL &&  found != true)
	{
		if(strcmp(travel->serviceName, message) == 0)
		{
			found = true;
		}
		travel = travel->next;
	}
	if(found == true)
	{
		//reply with IP address
		write(connfd, travel->serviceIP, 16);
		close(connfd);
	}
	else
	{
		//if data not in cache
		//ask closest DNS for data
		getDNS_Data(message, connfd);
		//send reply data to client
		//write(temp->connfd, temp->message, strlen(temp->message));
	}
	//close connection
	//close(temp->connfd);
	//close thread
	//pthread_exit(EXIT_SUCCESS);
	return NULL;
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
	//push((char*)addr->ai_addr->sa_data, (char*)IP);
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
					//write(connfd,"SUCCESS", strlen("SUCCESS"));
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
					//conn_t data;
					//data.connfd = connfd;
					//strcpy(data.message, message);
					//pthread_create(*thread, *attr, &runThread, connfd);
					getDNS_Data(message, connfd);
					//runThread(message, connfd);
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