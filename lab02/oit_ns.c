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

typedef struct list_element
{
    time_t keep_alive; //time stamp for keeping alive when i recieve keep alive message update time stamp -1 equals dead.
    char service_name[MAX_SERVICE_NAME_LEN + 1];
    uint16_t port;
} service_t;

int main(int argc, char *argv[])
{
    int servicePort = 0; //50000
    int minimumPorts = 100;
    int keepAliveTime = 300; //seconds
    int command = 0;
    char buffer[1024];
    int verbose = 0;
    request_t message;
    time_t current;
    socklen_t len;

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
    printf("\nfinished command line\n");
    service_t services[minimumPorts];

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    

    struct sockaddr_in myaddr;
    struct sockaddr_in recv_addr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(servicePort);
    len = sizeof(recv_addr);

    bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    int bound = sock2port(fd);
    printf("Bound: %d\n", bound);

    if (verbose == 1)
    {
        if(servicePort == 0)
        {
            servicePort = bound;
        }
        printf("\nService Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n\n",
               servicePort, minimumPorts, keepAliveTime);
    }
    while (1)
    {
        memset(&buffer, 0, sizeof(buffer));
        memset(&message, 0, sizeof(message));
        printf("\nstart_rec:\n");
        recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *) &recv_addr, &len);
        printf("\nrec\n");
        decode(&buffer, &message); //decode

        printf("Service_name: %s", message.service_name);
           // message.msg_type = RESPONSE;
           // message.status = ALL_PORTS_BUSY;
        memset(&buffer, 0, sizeof(buffer));
        encode(&message, &buffer);
        printf("Service_name: %s", ((request_t*)buffer)->service_name);
       // }
        //else 
       // {
        //    services[index].service_name = message.service_name;
        //    services[index].port = message.port;
        //    services[index].keep_alive = time(0);
        //    current = time(0);
       //     for (int i = 0; i <= minimumPorts; i++)
       //     {
   //          if (difftime(current, services[i].keep_alive) > keepAliveTime)
                //if((current - keepAliveTime) < services[i].keep_alive)
      //          {
      //              services[i].keep_alive = -1;
      //          }
      //      }
            //manage data
      //  }

        //process message, msg_type defines action if succeded success
        //encode msg_type=response status is depeinding
        //send
        printf("\nsending\n");
        sendto(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, len);
        printf("\nsent\n");
    }

    printf("Exiting");
    return (EXIT_SUCCESS);
}
