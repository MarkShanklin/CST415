/***********************************************************
* File Name: readline.c
* Purpose: Read a line it a time from file descriptor
* Creation Date: 10/02/2017
* Last Modified: 10/02/2017
* Created By: Mark Shanklin
**********************************************************/
#include "readline.h"
#include "readblocks.h"
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
    char *src_ptr = myBuff.buffer + myBuff.amountCopied;
    for(int i = 0; i < size; i++)
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
            myBuff.amountCopied++;;
            if(*bptr == '\n')
                break;
        } 
        else if (myBuff.amountRead <= 0)
        {
            if(bptr != buff)
               break; 
            return NULL;
        }
    }
    *bptr = '\0';
    return buff;
};