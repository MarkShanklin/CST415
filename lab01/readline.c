/***********************************************************
* File Name: readline.c
* Purpose: Read a line it a time from file descriptor
* Creation Date: 10/02/2017
* Last Modified: 10/02/2017
* Created By: Mark Shanklin
**********************************************************/
#include "readblocks.h"

typedef struct 
{
    int8_t placeHolder;
    char buffer[BLOCK_SIZE];
} buffer_t;

static __thread buffer_t myBuff;

char *readline(char *buff, ssize_t size, int fd)
{
    //int8_t amount;
    if(myBuff.placeHolder == 0)
    {
        myBuff.placeHolder = read_block(fd,myBuff.buffer); 
    }
    for(int i = myBuff.placeHolder; i < BLOCK_SIZE; i++)
    {
        
        
    }
};