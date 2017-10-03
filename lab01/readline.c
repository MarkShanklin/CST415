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
    memset(buff,0,size);
    for (int j = 0; j < size;)
    {
        if (myBuff.placeHolder == 0)
        {
            myBuff.amount = read_block(fd, myBuff.buffer);

            if((myBuff.amount) > size){
                memcpy(buff, myBuff.buffer, (size));
                myBuff.placeHolder += size;
                myBuff.amount -= (size);
                return buff;
            } 

            if (myBuff.amount <= 0)
            {
                return NULL;
            }

            for (int i = 0; i < myBuff.amount; i++)
            {
                if (myBuff.buffer[i] == '\n')
                {
                    myBuff.placeHolder = (i+1);
                    memcpy(buff+j, myBuff.buffer, (i+1));
                    myBuff.amount -= (i+1);
                    return buff;
                }
            }
            
            memcpy(buff+j, myBuff.buffer, myBuff.amount);
            j += myBuff.amount;
            myBuff.amount = 0;
            myBuff.placeHolder = 0;
            
        }
        else
        {
            if((myBuff.amount) > size){
                memcpy(buff, myBuff.buffer + myBuff.placeHolder, (size));
                myBuff.amount -= (size);
                myBuff.placeHolder += ((size));
                return buff;
            }
        
            for (int i = myBuff.placeHolder; i < (myBuff.amount - myBuff.placeHolder); i++)
            {
                if (myBuff.buffer[i] == '\n')
                {
                    memcpy(buff+j, (myBuff.buffer + myBuff.placeHolder), (i+1));
                    myBuff.amount = (myBuff.amount + myBuff.placeHolder) - (i+1);
                    myBuff.placeHolder = i+1;
                    return buff;
                }
                
            }
            memcpy(buff+j, myBuff.buffer + myBuff.placeHolder, myBuff.amount);
            j += myBuff.amount;
            myBuff.amount = 0;
            myBuff.placeHolder = 0;
        }
    }
    return buff;
};