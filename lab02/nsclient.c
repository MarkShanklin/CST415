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
    int minimumPorts = 100;
    int keepAliveTime = 300; //seconds
    int command = 0;
    int verbose = 0;

    while ((command = getopt(argc, argv, "p:n:t:hv")) != -1)
    {
        switch (command)
        {
        case 'p':
            servicePort = atoi(optarg);
            break;
        case 'n':
            minimumPorts = atoi(optarg);
            break;
        case 't':
            keepAliveTime = atoi(optarg);
            break;
        case 'h':
            printf("This program is a service used to maintain unique port numbers.\n\n"
                   "-p \t<service port>\n"
                   "-n \t<minimum number of ports>\n"
                   "-t \t<keep alive time in seconds\n");
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

    if (verbose == 1)
    {
        printf("Service Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n\n",
               servicePort, minimumPorts, keepAliveTime);
    }
    request_t message;
    int _error = 0; //needs to change
    socklen_t len;
    len = sizeof(struct sockaddr);
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("\nPlease enter msg: ");
        fgets(buffer, sizeof(buffer), stdin);
        strcpy(message.service_name,buffer);
        message.port = 9000;
        message.msg_type = DEFINE_PORT;
        message.status = SUCCESS;

        encode(&message,&buffer);
        printf("\nsending\n");
        _error = sendto(clientSocket_fd, &buffer, sizeof(buffer), 0,(struct sockaddr *) &serverAddr, len);
        if (_error < 0)
            fprintf(stderr,"ERROR in sendto");
        printf("\nsent");
        _error = recvfrom(clientSocket_fd, &message, sizeof(message), 0,(struct sockaddr *) &serverAddr, &len);
        if (_error < 0)
            fprintf(stderr,"ERROR in recvfrom");
        printf("rec");
        decode(&message,&message);

        printf("\nservice_name: %s", message.service_name);
        printf("\nstatus: %d", message.status);
        printf("\nmsg_type: %d", message.msg_type);
        printf("\nport: %d", message.port);
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}