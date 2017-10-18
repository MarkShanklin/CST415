/***********************************************************
* File Name     : nsclient.c
* Purpose       : Name Server Client Program
* Creation Date : 09-30-2016
* Last Modified : Tue 17 Oct 2017 03:14:23 PM PDT
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
    int clientSocket_fd = 0;;
    int _error = 0;
    int testCase = 0;
    int i = 0;
    int times = 0;
    char g_address[512] = "unix.cset.oit.edu";
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct addrinfo hints;
    struct addrinfo *addr;
    request_t message;
    socklen_t len;
    
    while ((command = getopt(argc, argv, "p:s:hv")) != -1)
    {
        switch (command)
        {
        case 'p':
            servicePort = atoi(optarg);
            break;
        case 's':
            strcpy(g_address,optarg);
            break;
        case 'h':
            printf("This program is a service used to"
                   " maintain unique port numbers.\n\n"
                   "-p \t<service port>\n");
            break;
        case 'v':
            verbose = 1;
        case '?':
            break;
        }
    }

    clientSocket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    len = sizeof(serverAddr);
    memset((char *)&serverAddr, 0, len);
    memset(&hints, 0, sizeof(hints));
    serverAddr.sin_family = AF_INET;
    hints.ai_family = AF_INET;
    if (getaddrinfo(g_address, NULL, &hints, &addr) != 0)
    {
        perror("Error getting address info: ");
    }
    memcpy(&serverAddr, addr->ai_addr, addr->ai_addrlen);
    serverAddr.sin_port = htons(servicePort);
    if (verbose == 1)
    {
        printf("Name Server Client\n");
        printf("Service Port: %d\n",servicePort);
    }

    while (1)
    {
        printf("\nPlease enter test number(1-6): ");
        fgets(buffer, sizeof(buffer), stdin);
        testCase = atoi(buffer);
        memset(buffer, 0, sizeof(buffer));

        //TEST 1
        //DEFINE_PORT multiple
        switch(testCase)
        {
            case DEFINE_PORT:
                times = 9999;
                for(i = 0; i < times; i++)
                {
                    sprintf(message.service_name, "Google%d",i);
                    message.msg_type = 1;
                    message.status = SUCCESS;
                    message.port = (uint16_t)i;
                    if (verbose == 1)
                    {
                        printf("\nrequest_t sent:");
                        printf("\nport: %d", message.port);
                        printf("\nmsg_type: %d", message.msg_type);
                        printf("\nstatus: %d", message.status);
                        printf("\nservice_name: %s", message.service_name);
                    }
                    if(encode(&message,&message) == NULL)
                    {
                        fprintf(stderr,"\nEncode returned NULL");
                    }
                    _error = sendto(    clientSocket_fd, 
                                        &message, 
                                        sizeof(request_t), 
                                        0,
                                        (struct sockaddr *) &serverAddr, 
                                        len
                                    );
                    if (_error < 0)
                        perror("ERROR in sendto");
                    memset(&message,'\0',sizeof(request_t));
                    _error = recvfrom(  clientSocket_fd, 
                                        &message, 
                                        sizeof(request_t), 
                                        0,
                                        (struct sockaddr *) &serverAddr, 
                                        &len
                                    );
                    if (_error < 0)
                        perror("ERROR in recvfrom");
                    if(decode(&message,&message) == NULL)
                    {
                        fprintf(stderr,"\nDecode returned a NULL");
                    }
                    else
                    {
                        if (verbose == 1)
                        {
                            fprintf(stderr,"\nrequest_t recieved:");
                            fprintf(stderr,"\nport: %d", message.port);
                            fprintf(stderr,"\nmsg_type: %d", message.msg_type);
                            fprintf(stderr,"\nstatus: %d", message.status);
                            fprintf(stderr,"\nservice_name: %s", message.service_name);
                        }
                        if(message.port != (uint16_t)i)
                        {
                            fprintf(stderr,"\nport mismatch got %d expected %d",
                        message.port, (uint16_t)i);
                        }
                        if(message.msg_type != RESPONSE)
                        {
                            fprintf(stderr,
                                    "\nmsg_type invalid expected 5 got %d",
                                    message.msg_type);
                        }
                        if(i < 100)
                        {
                            if(message.status != SUCCESS)
                            {
                                fprintf(stderr,
                                    "\nInvalid status expected 0 got: %d",
                                    message.status);
                            }
                        }
                        else
                        {
                            if(message.status != ALL_PORTS_BUSY)
                            {
                                fprintf(stderr,
                                    "\nInvalid status expected 3 got: %d",
                                    message.status);
                            }
                        }
                        sprintf(buffer, "Google%d",i);
                        if(strcmp(message.service_name, buffer) != 0)
                        {
                            fprintf(stderr,
                                "\nService name mismatch got %s expected %s",
                            message.service_name, buffer);
                        }

                    }
                }
                break;
            case LOOKUP_PORT:
                break;
            case KEEP_ALIVE:
                break;
            case CLOSE_PORT:
                break;
            case RESPONSE:
                break;
            case STOP:
                break;
            case 7: //sending not a request_t
                
                break;
            default:
                break;
        }

    }
    if(verbose == 1)
    {
        printf("Exiting");
    }
    return (EXIT_SUCCESS);
}
