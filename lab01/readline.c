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
    int8_t placeHolder;
    int8_t amount;
    char buffer[BLOCK_SIZE];
} buffer_t;

static __thread buffer_t myBuff;

char *readline(char *buff, ssize_t size, int fd)
{
    char *bptr = buff;
    char *src_ptr = myBuff.buffer;
    while( myBuff.amount < size || *bptr != '\n')
    {
        if(myBuff.placeHolder == myBuff.amount)
        {    
            myBuff.placeHolder = read_block(fd, myBuff.buffer);
            myBuff.amount = 0;
        }
        if(myBuff.placeHolder >= 0)
        {
            *bptr++ = *src_ptr++;
            myBuff.amount++;
        } 
        else if (myBuff.placeHolder < 0)
        {
            return NULL;
        }
    }
    
    // if(*bptr != '\n')
    // {
    //     myBuff.amount = 0;
    // } 
    // else 
    // {
    //     myBuff.placeHolder = 0;
    // }
    // *bptr++ = '\0';
    *bptr = '\0';
    return buff;

    // char *bptr = buff;
    // char *src_ptr = myBuff.buffer[myBuff.placeHolder];

    // for (int j = 0; j < size;)
    // {
    //     printf("j:%d",j);
    //     if (myBuff.placeHolder >= BLOCK_SIZE)
    //     {
    //         myBuff.amount = read_block(fd, myBuff.buffer);
    //         if(myBuff.amount < 0)
    //         {
    //             return NULL;
    //         }
    //         myBuff.placeHolder = 0;
    //         src_ptr = myBuff.buffer[myBuff.placeHolder];            
    //     }
    //     for (int i = myBuff.placeHolder; i < (myBuff.amount - myBuff.placeHolder) && j < size; i++)
    //     {
    //         *bptr++ = *src_ptr++;
    //         myBuff.placeHolder++;
    //         j++;
    //         if (*src_ptr == '\n')
    //         {
    //             *bptr++ = '\0';
    //             return buff;
    //         } 
    //     }
    // }
    // *bptr++ = '\0';
    // return buff;
};