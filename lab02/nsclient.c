/***********************************************************
* File Name     : nsclient.c
* Purpose       : Name Server Client Program
* Creation Date : 09-30-2016
* Last Modified : Fri 30 Sep 2016 11:17:08 AM PDT
* Created By    : Mark Shanklin 
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "nameserver.h"
#include "encode.h"

typedef struct list_element list_element;
struct list_element
{
    time_t keep_alive; //time stamp for keeping alive when i recieve keep alive message update time stamp -1 equals dead.
    char service_name[MAX_SERVICE_NAME_LEN + 1];
    uint16_t port;

    //list_element *next;
    //list_element *previous;
} service_t;

int main(int argc, char *argv[])
{
    int servicePort = 50000;
    int minimumPorts = 100;
    int keepAliveTime = 300; //seconds
    int command = 0;
    bool verbose = false;

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
            verbose = true;
        case '?':
            break;
        }
    }

    service_t services[minimumPorts];

    int clientSocket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[1024];
    struct sockaddr_in serverAddr;
    //struct sockaddr_in recv_addr;

    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(servicePort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Bound: %d", nstoh(serverAddr.sin_port));

    if (verbose)
    {
        printf("Service Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n\n",
               servicePort, minimumPorts, keepAliveTime);
    }
    request_t message;
    int _error = 0; //needs to change
    time_t current;
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("\nPlease enter msg: ");
        fgets(buffer, sizeof(buffer), stdin);
        strcpy(message.service_name,buffer);
        message.port = 9000;
        message.msg_type = DEFINE_PORT;
        message.status = SUCCESS;

        encode(&message,&message);

        _error = sendto(fd, message, sizeof(message), 0, &serverAddr, sizeof(serverAddr));
        if (n < 0)
            error("ERROR in sendto");

        _error = recvfrom(fd, message, sizeof(message), 0, &serverAddr, sizeof(serverAddr));
        if (n < 0)
            error("ERROR in recvfrom");

        decode(&message,&message);

        printf("\nservice_name: %s", message.service_name);
        printf("\nstatus: %d", message.status);
        printf("\nmsg_type: %d", message.msg_type);
        printf("\nport: %d", message.port);
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}