/***********************************************************
* File Name     : encode.c
* Purpose       : encode decode and validate data
* Creation Date : 09-26-2017
* Last Modified : Sat 30 Sep 2017 11:11:56 AM PDT
* Created By    : Mark Shanklin 
**********************************************************/
#include "nameserver.h"
#include <arpa/inet.h>
#include <string.h>
#include <stddef.h>

#define MSG_TYPE_ERROR 2
#define STATUS_ERROR 3
#define SERVICE_NAME_ERROR 4

void *encode(request_t *request, void *buff)
{
    if (request->msg_type > 6 || request->msg_type < 1)
        return NULL; //validate msg_type
    if (request->status > 5 || request->status < 0)
        return NULL; //validate status
    if (request != buff)
    {
        memcpy(request->service_name, ((request_t *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
        ((request_t *)buff)->msg_type = request->msg_type;             //copy msg type only one byte no need to network order
        ((request_t *)buff)->status = request->status;                 //copy status only one byte no need to network order
    }
    ((request_t *)buff)->service_name[MAX_SERVICE_NAME_LEN] = '\0'; //make last item in string NULL
    int length = strlen(request->service_name);                    //find the length up to the first NULL
    memset(buff + length, '\0', MAX_SERVICE_NAME_LEN - length);       //NULL fill from first NULL to end
    ((request_t *)buff)->port = htons(request->port);              //copy port using network byte order
   
    return buff;
};

int is_invalid(request_t *request)
{
    if (request->msg_type > 6 || request->msg_type < 1)
    {
        return MSG_TYPE_ERROR;
    }
    if (request->status > 5 || request->status < 0)
        return STATUS_ERROR;

    if (request->service_name[MAX_SERVICE_NAME_LEN] != '\0')
    {
        return SERVICE_NAME_ERROR;
    }
    for (int i = 0; i < MAX_SERVICE_NAME_LEN; i++)
    {
        if  (request->service_name[i] == '\0')
        {
            for(int j = i + 1; j < MAX_SERVICE_NAME_LEN; j++, i++)
            {
                if (request->service_name[j] != '\0')
                {
                    return SERVICE_NAME_ERROR;
                }
            }
        }
    }
    return 0;
};

request_t *decode(void *buff, request_t *decoded)
{
    if (is_invalid((request_t *)buff))
        return NULL;
    if (buff != decoded)
    {
        memcpy(decoded->service_name, ((request_t *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
        ((request_t *)buff)->status = decoded->status;     //copy status only one byte no need to host order
        decoded->msg_type = ((request_t *)buff)->msg_type; //copy msg type only one byte no need to network order
    }
    decoded->port = ntohs(((request_t *)buff)->port);  //copy port using host byte order
    return buff;
};
