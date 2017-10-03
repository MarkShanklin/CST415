/***********************************************************
* File Name: readline.c
* Purpose: Read a line it a time from file descriptor
* Creation Date: 10/02/2017
* Last Modified: 10/02/2017
* Created By: Mark Shanklin
**********************************************************/
#include "readblocks.h"
#include "string.h"
#include <stdio.h>

typedef struct
{
    int8_t amountCopied;
    int8_t amountRead;
    int8_t totalCopied;
    char buffer[BLOCK_SIZE];
} buffer_t;

static __thread buffer_t myBuff;

char *readline(char *buff, ssize_t size, int fd)
{
    char *bptr = buff;
    char *src_ptr = myBuff.buffer;
    while( myBuff.totalCopied < size || *bptr != '\n')
    {
        if(myBuff.amountCopied == myBuff.amountRead)
        {    
            myBuff.amountRead = read_block(fd, myBuff.buffer);
            myBuff.amountCopied = 0;
            src_ptr = myBuff.buffer;
        }

        if(myBuff.amountRead > 0)
        {
            *bptr++ = *src_ptr++;
            myBuff.amountCopied++;
            myBuff.totalCopied++;
        } 
        else if (myBuff.amountRead < 0)
        {
            myBuff.amountCopied = 0;
            myBuff.totalCopied = 0;
            return NULL;
        }
    }

    return buff;
};