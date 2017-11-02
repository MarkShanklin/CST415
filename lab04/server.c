/***********************************************************
* File Name     : sever.c
* Purpose       : file server.
* Creation Date : 10-21-2017
* Last Modified : Tue 01 Nov 2017 11:43:34 PM PDT
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
#include "tscounter.h"
#include "getport.h"
#include "md5sum.h"

#define PORT 50050
#define MAX_CONS 5

typedef enum {false,true} bool;
static char serviceName[MAX_SERVICE_NAME_LEN + 1];
static int port;

static bool checkType(char* file)
{
    int len = strlen(file);
    if(len >= 3)
    {
        if(file[len-2] == '.' && file[len-1] == 'c'){ return true; }
        if(file[len-2] == '.' && file[len-1] == 'h'){ return true; }
    }
    if(len >= 5)
    {
        if(file[len-4] == '.' && file[len-3] == 'c' && file[len-2] == 'p' && file[len-1] == 'p'){ return true; }
        if(file[len-4] == '.' && file[len-3] == 't' && file[len-2] == 'x' && file[len-1] == 't'){ return true; }
    }
    return false;
}

static int Files(char* loc)
{
    DIR *dirPointer;
    struct dirent *entPointer;

    dirPointer = opendir (loc);
    if (dirPointer != NULL)
    {
        while (entPointer = readdir (dirPointer))
            if(checkType(entPointer->d_name))
                moveFiles(entPointer->d_name, loc);
        (void) closedir (dirPointer);
    }
    else
        perror ("ERROR OPENING DIRECTORY");
    return 0;
}

int moveFiles(char* file, char* path)
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in recvAddr ;
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*)&serverAddr, '0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    bind(connfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    int bound = sock2port(fd);
    printf("\nBound to port: %d", bound);

    socklen_t recvLen = sizeof(recvAddr );
    char file_path[45];
    sprintf(file_path, "%s/%s", path, file);
    FILE* fPointer;
    fopen(file_path, "r");

    char sendBuff[1024];

    listen(listenfd, 10);
    while ( fscanf(fPointer, "%s1023", &sendBuff))
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        write(connfd, sendBuff, strlen(sendBuff));
        close(connfd);
    }
}

int main()
{
    //initialization the name server
    setup_ns(NULL, PORT);
    tsc_reset();

    //connect to the name server
    port = request_port(serviceName);

    struct sockaddr_in serverAddr;
    struct sockaddr_in recvAddr ;


    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*)&serverAddr, '0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    int bound = sock2port(fd);
    printf("\nBound to port: %d", bound);

    char message[500];
    memset(message, '0', sizeof(sendBuff)); 
    socklen_t recvLen = sizeof(recvAddr r);
    int numRead;
    bool Run = true;

    listen(listenfd, 10);

    while(Run)
    {
        if(tsc_value() < MAX_CONS)
        {
            int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
            numRead= read(connfd, &message, sizeof(message));
            if(numRead > -1)
            {
                if (strcmp("exit", message) == 0)
                {
                    //exit
                    release_port();
                    tsc_decrement();
                }
                else if(strcmp("shutdown", message) == 0)
                {
                    tsc_reset();
                    Run = false;
                }
                else if(strlen(message) >= 5)
                {
                    if(message[0] == 'g' && message[1] == 'e' && message[2] == 't')
                    {
                        files(message);
                        tsc_increment();
                    }
                }
            }
            else
            {
                fprintf(ftderr,"ERROR");
            }
            close(connfd);
        }
    }

    release_port(serviceName, port);
    return 0;
}