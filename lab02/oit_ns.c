/***********************************************************
* File Name     : oit_ns.c
* Purpose       : Program used to maintain unique service names.
* Creation Date : 10-06-2017
* Last Modified : Tue 17 Oct 2017 01:59:34 PM PDT
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
    //used to store the port of the service.
    uint16_t port;
    //time stamp for keeping alive when i recieve keep alive message 
    //update time stamp -1 equals dead.
    time_t keep_alive;
    //used to store the services name.
    char service_name[MAX_SERVICE_NAME_LEN + 1]; 
} service_t;

int main(int argc, char *argv[])
{   //50000. bound to port zero so that it will assign a port
    int servicePort = 0;
    //the minimum and maximum number of ports my nameserver can store.
    int minimumPorts = 100;
    //seconds before marking the service dead. 
    int keepAliveTime = 300; 
    //used to handle comand line argurements.
    int command = 0; 
    //used to signify if you want to print out data that
    // is being process to the stdout.
    int verbose = 0;
     //used to store a recieved message.
    request_t message;
    //used to keep a instance of current time.
    time_t current;
    //used to store the length of a sockaddr.
    socklen_t len; 
    //access point variables
    //used as a loop control variable.
    int i; 
    //used to store the index to the first open port in the array.
    int first_openPort = 0; 
    //used to store the first dead port in the array that 
    //will be used if no open ports are available.
    int first_deadPort = 0; 
    //used to identify if an item is found in the array.
    int portTaken_found = 0; 

    //command line parsing.
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
            printf("This program is a service used to "
                   "maintain unique port numbers.\n\n"
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
    //used to store services and there ports.
    service_t services[minimumPorts];
    //setting up socket to a file discriptor.
    int fd = socket(AF_INET, SOCK_DGRAM, 0); 
    //used for the address of this program the name server.
    struct sockaddr_in myaddr;
    //used for the address of the client talking to this program.
    struct sockaddr_in client_addr; 
    //making sure that the struct is null filled.
    memset((char *)&myaddr, 0, sizeof(myaddr));
    //setting the servers family.
    myaddr.sin_family = AF_INET;
    //setting the global address to listen to anything.
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //setting the port on which to comunicate with the server.
    myaddr.sin_port = htons(servicePort);
    //storing the length of the clients address.
    len = sizeof(client_addr);
    //int used to store and track errors in the  read and send.
    int _error = 0;
    //binding to the fd to create a connection to listen on.
    bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    //used to print out the address that was bound to.
    int bound = sock2port(fd);
    //used to process a stop message and exit the while loop.
    int stop = 0;
    printf("OIT Name Server\n");
    printf("Bound to port: %d\n", bound);
    if (verbose == 1)
    {
        if(servicePort == 0)
        {
            //used to print out the assigned port
            servicePort = bound; 
        }
        printf("\nService Port: %d\n"
               "Minimum Ports: %d\n"
               "Keep Alive Time: %d\n",
               servicePort, minimumPorts, keepAliveTime);
    }
    //check to see if a stop command was recieved
    while (stop == 0)
    {
        if (verbose == 1)
        {
            printf("\nRecieving\n");
        }
        _error = recvfrom(
                            fd, 
                            &message, 
                            sizeof(request_t), 
                            0, 
                            (struct sockaddr *)&client_addr, 
                            &len
                         );
        if (verbose == 1)
        {
            printf("\nRecieved\n");
        }
        //check that the packect was size of a req
        if(_error == sizeof(request_t))
        {
            if(decode(&message, &message) != NULL)
            {
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
                //get the current time to compare time stamps
                current = time(0);

                for (i = 0; i < MAX_SERVICE_NAME_LEN; i++)
                {
                    time_t temp = services[i].keep_alive;
                    //if service is marked not alive
                    if(temp < 0) 
                    {
                        if(first_deadPort == -1)
                        {
                            first_deadPort = i;
                        }
                    }
                    //if service was never assigned in the array
                    else if(temp == 0)
                    {
                        if(first_openPort == -1)
                        {
                            first_openPort = i;
                        }
                    }
                    //if service is alive
                    else
                    {
                        //compare time stamps to see if needs to be marked as
                        //not alive
                        if(difftime(current,temp) > keepAliveTime)
                        {
                            services[i].keep_alive = -1;
                        }
                    }
                    //keeping track if a service is alive and found in the array
                    if((temp > 0) && (strcmp(services[i].service_name, 
                        message.service_name) == 0))
                    {
                        portTaken_found = i;
                    }
                }
                //assign open port to the first dead port is no open 
                //ports were found in the array. when i say port i mean spots
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
                    printf("\nFirst Dead Port(-1=NA): %d", first_deadPort);
                    printf("\nFirst Open(-1=NA): %d", first_openPort);
                    printf("\nPort Exsists in index(-1=NA): %d\n", 
                            portTaken_found);
                }
                //begin prossesing the message
                switch(message.msg_type)
                {
                    case DEFINE_PORT:
                        //if service doesn't already exsist
                        if(portTaken_found < 0)
                        {
                            //if no open port are available
                            if(first_openPort < 0)
                            {
                                message.status = ALL_PORTS_BUSY;
                            }
                            else
                            {
                                services[first_openPort].keep_alive = time(0);
                                strcpy(services[first_openPort].service_name, 
                                    message.service_name);
                                services[first_openPort].port = message.port;
                                message.status = SUCCESS;
                            }
                        }
                        //if service already exists
                        else
                        {
                            message.status = SERVICE_IN_USE;
                        }
                        break;
                    case LOOKUP_PORT:
                        //service is found
                        if(portTaken_found > -1)
                        {
                            message.status = SUCCESS;
                            message.port = services[portTaken_found].port;
                        }
                        //service was not found
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
                                strcpy(services[first_openPort].service_name, 
                                    message.service_name);
                                services[first_openPort].port = message.port;
                                message.status = SUCCESS;
                            }
                        }
                        break;
                    case CLOSE_PORT:
                        if(portTaken_found > -1)
                        {
                            if(message.port == services[portTaken_found].port)
                            {
                                services[portTaken_found].keep_alive = -1;
                                memset(
                                    services[portTaken_found].service_name, 
                                    '\0', MAX_SERVICE_NAME_LEN);
                                services[portTaken_found].port = -1;
                                message.status = SUCCESS;
                            }
                            else
                            {
                                message.status = INVALID_ARG;
                            }
                        }
                        else
                        {
                            message.status = SERVICE_NOT_FOUND;
                        }
                        break;
                    case RESPONSE:
                        message.status = INVALID_ARG;
                        break;
                    case STOP:
                        //For Hackathon
                        message.status = INVALID_ARG;
                        //Normal Operation
                        //message.status = SUCCESS;
                        //stop = 1;
                        break;
                }
            }
            else
            {
                if (verbose == 1)
                {
                    printf("\nDecode returned: NULL");
                }
                perror("\nDecode returned: NULL");
                message.status = INVALID_ARG;
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
        }
        else //packet not a request_t
        {
            message.status = INVALID_ARG;
        }
        //preparing message for sending
        if(encode(&message, &message) == NULL)
        {
            message.status = UNDEFINED_ERROR;
            perror("\nEncode returned: NULL");
            if (verbose == 1)
            {
                printf("\nEncode returned: NULL");
                printf("\nstatus: %d", message.status);
            }
        }
        if (verbose == 1)
        {
            printf("\nSending");
        }
        _error = sendto(
                            fd, 
                            &message, 
                            sizeof(request_t), 
                            0, 
                            (struct sockaddr *)&client_addr, 
                            len
                        );
        if(_error != sizeof(request_t))
        {
            perror("Sendto failed to send valid data");
        }
        else
        {
            if (verbose == 1)
            {
                printf("\nSent\n");
            }
        }
        
    }
    if(verbose == 1)
    {
        printf("Exiting");
    }
    return (EXIT_SUCCESS);
}
