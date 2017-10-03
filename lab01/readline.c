/***********************************************************
* File Name: readline.c
* Purpose: Read a line it a time from file descriptor
* Creation Date: 10/02/2017
* Last Modified: 10/02/2017
* Created By: Mark Shanklin
**********************************************************/
#include "readblocks.h"
#include "string.h"

typedef struct
{
    int8_t placeHolder;
    int8_t amount;
    char buffer[BLOCK_SIZE];
} buffer_t;

static __thread buffer_t myBuff;

char *readline(char *buff, ssize_t size, int fd)
{
    for (int j = 0; j < size;)
    {
        if (myBuff.placeHolder == 0)
        {
            myBuff.amount = read_block(fd, myBuff.buffer);

            if((j + myBuff.amount) > size){
                memcpy(buff+j, &myBuff.buffer, (size-j));
            } 

            if (myBuff.amount <= 0)
            {
                return NULL;
            }
            for (int i = 0; i < myBuff.amount; i++)
            {
                if (myBuff.buffer[i] == '\n')
                {
                    myBuff.placeHolder = i;
                    memcpy(buff+j, &myBuff.buffer, i + 1);
                    myBuff.amount -= myBuff.placeHolder;
                    j += i;
                    return buff;
                }
                else if (i == myBuff.amount - 1)
                {
                    myBuff.placeHolder = i;
                    memcpy(buff+j, &myBuff.buffer, i + 1);
                    myBuff.amount -= myBuff.placeHolder;
                    j += i;
                }
            }
        }
        else
        {
            if((j + myBuff.amount) > size){
                memcpy(buff+j, &myBuff.buffer, (size-j));
            }
        
            for (int i = myBuff.placeHolder; i < myBuff.amount; i++)
            {
                if (myBuff.buffer[i] == '\n')
                {
                    memcpy(buff+j, &myBuff.buffer + myBuff.placeHolder, i + 1);
                    myBuff.placeHolder = i;
                    myBuff.amount = myBuff.amount - myBuff.placeHolder;
                    j += i;
                    return buff;
                }
                else if (i == myBuff.amount - 1)
                {
                    memcpy(buff+j, &myBuff.buffer + myBuff.placeHolder, i + 1);
                    myBuff.placeHolder = i;
                    myBuff.amount = myBuff.amount - myBuff.placeHolder;
                    j += i;
                }
            }
        }
    }
    return buff;
};