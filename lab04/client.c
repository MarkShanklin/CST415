#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr,"\n ERROR : COULDN'T CREATE SOCKET \n");
        return EXIT_FAILURE;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(50050); 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       fprintf(stderr,"\n ERROR : CONNECT FAILED \n");
       return EXIT_FAILURE;
    }


    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        //do the test needed for the lab

        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            fprintf(stderr,"\n ERROR : FPUTS ERROR\n");
        }
    } 
    if(n < 0)
    {
        fprintf(stderr,"\n ERROR : READ ERROR \n");
    }

    return EXIT_SUCCESS;
}