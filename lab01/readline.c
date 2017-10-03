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
    char buffer[BLOCK_SIZE];
} buffer_t;

static __thread buffer_t myBuff;

char *readline(char *buff, ssize_t size, int fd)
{
    char *bptr = buff;
    char *src_ptr = myBuff.buffer;
    while( myBuff.amountCopied < size || *src_ptr != '\n')
    {
        if(myBuff.amountRead == myBuff.amountCopied)
        {    
            myBuff.amountRead = read_block(fd, myBuff.buffer);
            myBuff.amountCopied = 0;
        }
        if(myBuff.amountRead > 0)
        {
            *bptr++ = *src_ptr++;
            myBuff.amountCopied++;
        } 
        else if (myBuff.amountRead <= 0)
        {
            return NULL;
        }
    }
    *bptr = '\0';
    return buff;
};