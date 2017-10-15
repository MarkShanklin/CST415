/***********************************************************
* File Name     : nsclient.c
* Purpose       : Name Server Client Program
* Creation Date : 09-30-2016
* Last Modified : Fri 30 Sep 2016 11:17:08 AM PDT
* Created By    : Mark Shanklin 
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "nameserver.h"
#include "encode.h"

int main(int argc, char *argv[])
{
    int servicePort = 50000;
    int command = 0;
    int verbose = 0;

    while ((command = getopt(argc, argv, "p:hv")) != -1)
    {
        switch (command)
        {
        case 'p':
            servicePort = atoi(optarg);
            break;
        case 'h':
            printf("This program is a service used to maintain unique port numbers.\n\n"
                   "-p \t<service port>\n");
            break;
        case 'v':
            verbose = 1;
        case '?':
            break;
        }
    }
    int clientSocket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[1024];
    struct sockaddr_in serverAddr;
    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(servicePort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    request_t message;
    int _error = 0;
    socklen_t len;
    len = sizeof(serverAddr);

    if (verbose == 1)
    {
        printf("Service Port: %d\n",servicePort);
    }
    while (1)
    {
        printf("\nPlease enter ServiceName: ");
        fgets(buffer, sizeof(buffer), stdin);
        strcpy(message.service_name,buffer);
        message.port = 9000;
        message.msg_type = DEFINE_PORT;
        message.status = SUCCESS;
        if (verbose == 1)
        {
            printf("\nrequest_t sent:");
            printf("\nport: %d", message.port);
            printf("\nmsg_type: %d", message.msg_type);
            printf("\nstatus: %d", message.status);
            printf("\nservice_name: %s", message.service_name);
        }
        encode(&message,&message);
        _error = sendto(clientSocket_fd, &message, sizeof(request_t), 0,(struct sockaddr *) &serverAddr, len);
        if (_error < 0)
            fprintf(stderr,"ERROR in sendto");
        _error = recvfrom(clientSocket_fd, &message, sizeof(request_t), 0,(struct sockaddr *) &serverAddr, &len);
        if (_error < 0)
            fprintf(stderr,"ERROR in recvfrom");
        decode(&message,&message);
        if (verbose == 1)
        {
            printf("\nrequest_t recieved:");
            printf("\nport: %d", message.port);
            printf("\nmsg_type: %d", message.msg_type);
            printf("\nstatus: %d", message.status);
            printf("\nservice_name: %s", message.service_name);
        }
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}