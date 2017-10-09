/***********************************************************
* File Name     : oit_ns.c
* Purpose       : Service used to maintain unique port numbers.
* Creation Date : 09-27-2016
* Last Modified : Mon 10 Oct 2016 11:59:34 AM PDT
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
#include <bool.h>

#include "nameserver.h"

typedef struct list_element list_element;
struct list_element 
{
    int port;
    int keep_alive;
    struct sockaddr_in IP;
    char *serive_name;
    list_element *next;
    list_element *previous;
}service_t;

int main(int argc, char *argv[])
{
    int servicePort = 30000;
    int startClientPort = 40000;
    int endClientPort = 40099;
    int keepAliveTime = 300;
    int command = 0;
    char buffer[1000];
    bool verbose = false;
    
    while((command = getopt(argc, argv, "p:s:e:t:hv")) != -1)
    {
        switch(command)
        {   
            case 'p':
                servicePort = atoi(optarg);
                break;
            case 's':
                startClientPort = atoi(optarg);
                break;
            case 'e':
                endClientPort = atoi(optarg);
                break;
            case 't':
                keepAliveTime = atoi(optarg);
                break;
            case 'h':
                printf( "This program is a service used to maintain unique port numbers.\n\n"
                        "-p \t<service port>\n"
                        "-s \t<starting client port number>\n"
                        "-e \t<ending client port number>\n"
                        "-t \t<keep alive time in seconds\n");
                break;
            case 'v':
                verbose = true;
            case '?':
                break;
        }
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in myaddr;
    struct sockaddr_in recv_addr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(servicePort);

    bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    
    if(verbose)
    {
        printf("Service port: %d\n"
               "Starting client port: %d\n"
               "Ending client port: %d\n"
               "Keep Alive time: %d\n\n", servicePort, startClientPort, endClientPort, keepAliveTime);
    }
    
    while(true)
    { recvfrom(fd, buffer, sizeof(buffer), 0, recv_addr, sizeof(recv_addr));

    }

    
    printf("Exiting");
    return (EXIT_SUCCESS);
}
