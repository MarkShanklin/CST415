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
    printf("ph: %d\na: %d\n", myBuff.placeHolder, myBuff.amount);
    for (int j = 0; j < size;)
    {
        printf("J: %d ", j);
        if (myBuff.placeHolder == 0)
        {
            myBuff.amount = read_block(fd, myBuff.buffer);

            if((j + myBuff.amount) > size){
                memcpy(buff+j, &myBuff.buffer, (size-j));
                myBuff.placeHolder = size - j;
                myBuff.amount -= size-j;
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
                    myBuff.placeHolder = i+1;
                    memcpy(buff+j, &myBuff.buffer, i+1);
                    myBuff.amount -= i+1;
                    return buff;
                }
            }
            
            memcpy(buff+j, &myBuff.buffer, myBuff.amount);
            j += myBuff.amount - 1;
            myBuff.amount = 0;
            myBuff.placeHolder = 0;
            
        }
        else
        {
            if((j + myBuff.amount) > size){
                memcpy(buff+j, &myBuff.buffer + myBuff.placeHolder, (size-j));
                myBuff.amount -= size-j;
                myBuff.placeHolder = size-j;
                return buff;
            }
        
            for (int i = myBuff.placeHolder; i < (myBuff.amount + myBuff.placeHolder); i++)
            {
                printf("bottomi: %d ", i);
                if (myBuff.buffer[i] == '\n')
                {
                    memcpy(buff+j, &myBuff.buffer + myBuff.placeHolder, i);
                    myBuff.placeHolder = i+1;
                    myBuff.amount -= (i - myBuff.placeHolder);
                    return buff;
                }
                
            }
            memcpy(buff+j, &myBuff.buffer + myBuff.placeHolder, myBuff.amount);
            myBuff.placeHolder = 0;
            j += myBuff.amount;
            myBuff.amount = 0;
        }
    }
    return buff;
};