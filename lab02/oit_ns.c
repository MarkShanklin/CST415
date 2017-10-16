/***********************************************************
* File Name     : oit_ns.c
* Purpose       : Service used to maintain unique port numbers.
* Creation Date : 09-27-2016
* Last Modified : Mon 10 Oct 2016 11:59:34 AM PDT
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
#include "encode.h"
#include "addr2str.h"

typedef struct
{
    uint16_t port;
    time_t keep_alive; //time stamp for keeping alive when i recieve keep alive message update time stamp -1 equals dead.
    char service_name[MAX_SERVICE_NAME_LEN + 1];
} service_t;

int main(int argc, char *argv[])
{
    int servicePort = 0; //50000
    int minimumPorts = 100;
    int keepAliveTime = 300; //seconds
    int command = 0;
    int verbose = 0;
    request_t message;
    time_t current;
    socklen_t len;

    //access point variables
    int i;
    int first_openPort = 0;
    int first_deadPort = 0;
    int portTaken_found = 0;

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
    service_t services[minimumPorts];
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in myaddr;
    struct sockaddr_in client_addr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(servicePort);
    len = sizeof(client_addr);
    int _error = 0;
    bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    int bound = sock2port(fd);
    printf("OIT Name Server\n");
    printf("Bound to port: %d\n", bound);

    if (verbose == 1)
    {
        if(servicePort == 0)
        {
            servicePort = bound;
        }
        printf("\nService Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n",
               servicePort, minimumPorts, keepAliveTime);
    }
    while (1)
    {
        _error = recvfrom(fd, &message, sizeof(request_t), 0, (struct sockaddr *)&client_addr, &len);
        if(_error == sizeof(request_t))
        {
            decode(&message, &message); //decode
            if (verbose == 1)
            {
                printf("\nrequest_t recieved:");
                printf("\nport: %d", message.port);
                printf("\nmsg_type: %d", message.msg_type);
                printf("\nstatus: %d", message.status);
                printf("\nservice_name: %s", message.service_name);
            }
            first_deadPort = -1;
            first_openPort = -1;
            portTaken_found = -1;
            current = time(0);

            for (i = 0; i < MAX_SERVICE_NAME_LEN; i++)
            {
                time_t temp = services[i].keep_alive;
                if(temp < 0)
                {
                    if(first_deadPort == -1)
                    {
                        first_deadPort = i;
                    }
                }
                else if(temp == 0)
                {
                    if(first_openPort == -1)
                    {
                        first_openPort = i;
                    }
                }
                else
                {
                    if(difftime(current,temp) > keepAliveTime)
                    {
                        services[i].keep_alive = -1;
                    }
                }
                if(temp > 0 && (strcmp(services[i].service_name, message.service_name) == 0))
                {
                    portTaken_found = i;
                }
            }
            if(first_openPort == -1 && first_deadPort > -1)
            {
                first_openPort = first_deadPort;
            }
            if (verbose == 1)
            {
                printf("\nCurrent Data:");
                printf("\nport: %d", message.port);
                printf("\nmsg_type: %d", message.msg_type);
                printf("\nservice_name: %s", message.service_name);
                printf("\nFirst Dead Port: %d", first_deadPort);
                printf("\nFirst Open: %d", first_openPort);
                printf("\nPort Exsists in index: %d\n", portTaken_found);
            }
            switch(message.msg_type)
            {
                case DEFINE_PORT:
                    if(portTaken_found < 0)
                    {
                        if(first_openPort < 0)
                        {
                            message.status = ALL_PORTS_BUSY;
                        }
                        else
                        {
                            services[first_openPort].keep_alive = time(0);
                            strcpy(services[first_openPort].service_name, message.service_name);
                            services[first_openPort].port = message.port;
                            message.status = SUCCESS;
                        }
                    }
                    else
                    {
                        message.status = SERVICE_IN_USE;
                    }
                    break;
                case LOOKUP_PORT:
                    if(portTaken_found > -1)
                    {
                        message.status = SUCCESS;
                        message.port = services[portTaken_found].port;
                    }
                    else
                    {
                        message.status = SERVICE_NOT_FOUND;
                    }
                    break;
                case KEEP_ALIVE:
                    if(portTaken_found > -1)
                    {
                        services[portTaken_found].keep_alive = time(0);
                        message.status = SUCCESS;
                    }
                    else
                    {
                        if(first_openPort < 0)
                        {
                            message.status = ALL_PORTS_BUSY;
                        }
                        else
                        {
                            services[first_openPort].keep_alive = time(0);
                            strcpy(services[first_openPort].service_name, message.service_name);
                            services[first_openPort].port = message.port;
                            message.status = SUCCESS;
                        }
                    }
                    break;
                case CLOSE_PORT:
                    if(portTaken_found > -1)
                    {
                        services[portTaken_found].keep_alive = -1;
                        memset(services[portTaken_found].service_name, '\0', MAX_SERVICE_NAME_LEN);
                        services[portTaken_found].port = -1;
                        message.status = SUCCESS;
                    }
                    else
                    {
                        message.status = SERVICE_NOT_FOUND;
                    }
                    break;
                case STOP:
                    return (EXIT_SUCCESS);
                    break;
            }
            message.msg_type = RESPONSE;
            if (verbose == 1)
            {
                printf("\nrequest_t sent:");
                printf("\nport: %d", message.port);
                printf("\nmsg_type: %d", message.msg_type);
                printf("\nstatus: %d", message.status);
                printf("\nservice_name: %s", message.service_name);
            }
            encode(&message, &message);
            _error = sendto(fd, &message, sizeof(request_t), 0, (struct sockaddr *)&client_addr, len);
        }
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}
