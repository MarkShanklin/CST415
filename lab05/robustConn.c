/***********************************************************
* File Name     : robustConn.c
* Purpose       : A more robust connection strategy.
* Creation Date : 11-07-2017
* Last Modified : Tue 07 Nov 2017 03:53:25 PM PST
* Created By    : Mark Shanklin 
***********************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>

/***********************************************************
* a function to time out a read
***********************************************************/
static int timed_read(int sockfd, int timeout, char *buf, int count)
{
    int result;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0;

    result = select(sockfd+1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
    if(result > 0)
    {
        return read(sockfd, buf, count);
    }
    else
    {
        return result;
    }
}

//*************************************************************
int main(int argc, char *argv[])
{
    //used to handle comand line argurements.
    int command = 0; 
    //used to signify if you want to print out data that
    // is being process to the stdout.
    int verbose = 0;
    char server_name[256] = "bbc.com";
    int server_socket;
    struct addrinfo *servinfo, *p;
    int ret_val;
    char port[15] = "80";
    char responce[256];
    const char *message = "GET / HTTP/1.1\r\nHost: bbc.com\r\nConnection:" 
            "keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent:"
            " Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML,"
            " like Gecko) Chrome/61.0.3163.100 Safari/537.36\r\nAccept:"
            " text/html,application/xhtml+xml,application/xml;q=0.9,image"
            "/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate"
            "\r\nAccept-Language: en-US,en;q=0.8\r\n\r\n";
    
    //command line parsing.
    while ((command = getopt(argc, argv, "hv")) != -1) 
    {
        switch (command)
        {
        case 'h':
            printf( "This program is a robust connection to "
                    "a service.\n\n");
            break;
        case 'v':
            verbose = 1;
        case '?':
            break;
        }
    }

    //get host name from command line args
    //server_name = ;

    if(ret_val = getaddrinfo(server_name,port,NULL,&servinfo) != 0)
    {
        fprintf(stderr,"getaddrinfo: %d\n", ret_val);
        exit(1);
    }
    char dst[INET6_ADDRSTRLEN];
    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        switch(p->ai_socktype)
        {
            case 17:
                fprintf(stdout,"UDP");
                break;
            case 6;
                fprintf(stdout,"TCP");
                break;
            case 0:
                fprintf(stdout,"RAW");
                break;
        }
        fprintf(stdout," connection to %s",inet_ntop(p->ai_family, p->ai_addr,
            dst, INET6_ADDRSTRLEN));
        if((server_socket = 
            socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            fprintf(stderr,"Failed.");
        }
        else if(connect(server_socket, p->ai_addr, p->addrlen) != 0)
        {
            fprintf(stderr,"Failed.");
        }
        else
        {
            write(server_socket,message,sizeof(message));
            if(timed_read(server_socket, 5, responce, 256) > 0)
            {
               fprintf(stdout, "Succeeded. Communicated.");
            }
            else{
                fprintf(stderr, "Failed.");
            }
        }
    }

    //free address info
    freeaddrinfo(&servinfo);

    if(verbose == 1)
    {
        fprintf(stdout,"\nExiting with Success");
    }
    return (EXIT_SUCCESS);
};
