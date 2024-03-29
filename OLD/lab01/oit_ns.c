/***********************************************************
* File Name     : oit_ns.c
* Purpose       : Service used to maintain unique port numbers.
* Creation Date : 09-27-2016
* Last Modified : Sat 14 Oct 2017 03:44:26 PM PDT
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

typedef struct list_element list_element;
typedef struct list_element 
{
    int keep_alive; //time stamp for keeping alive when i reqieve keep alive message update time stamp -1 equals dead.
    char service_name[MAX_SERVICE_NAME_LEN + 1];
    uint16_t port;
}service_t;

int main(int argc, char *argv[])
{
    int servicePort = 50000;
    int minimumPorts = 100;
    int keepAliveTime = 300; //seconds
    int command = 0;
    char buffer[1024];
    int verbose = 0;
 
    while((command = getopt(argc, argv, "p:n:t:hv")) != -1)
    {
        switch(command)
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
                printf( "This program is a service used to maintain unique port numbers.\n\n"
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
    struct sockaddr_in recv_addr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(servicePort);

    bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    
    if(verbose == 1)
    {
        printf("Service Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n\n", servicePort, minimumPorts, keepAliveTime);
    }
    
    while(1)
    { recvfrom(fd, buffer, sizeof(buffer), 0, &recv_addr, sizeof(recv_addr));
        //decode
        //process message, msg_type defines action if succeded success
        //encode msg_type=response status is depeinding
        //send
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}
