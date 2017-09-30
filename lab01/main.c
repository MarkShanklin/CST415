/***********************************************************
* File Name     : main.c
* Purpose       : 
* Creation Date : 09-26-2017
* Last Modified : Sat 30 Sep 2017 11:17:03 AM PDT
* Created By    : Mark Shanklin 
**********************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "nameserver.h"
#include "encode.h"

int main(void)
{
    request_t req;
    request_t buff;
    memset(req.service_name, 0, MAX_SERVICE_NAME_LEN + 1);
    memset(buff.service_name, 0, MAX_SERVICE_NAME_LEN + 1);
    char *_ptr;
    for (int i = 0; i < 10; i++)
    {
        req.msg_type = i;
        req.status = i;
        strcpy(req.service_name, "this is the service name" + i);
        req.port = 1000 + i;
        _ptr = encode(&req, &buff);

        printf("buff_ptr: %010x\n", &_ptr);
        printf("req: %010x\n", &req);
        printf("req_msg_type: %d\n", req.msg_type);
        printf("req_service_name: ");
        for (int j = 0; j < MAX_SERVICE_NAME_LEN + 1; j++)
        {
            if(req.service_name[j] == '\0')
            printf("0");
            else
            printf("%c",req.service_name[j]);
        }
        printf("\n");
        printf("req_port: %d\n", req.port);
        printf("req_status: %d\n", req.status);

        printf("buff: %010x\n", &buff);
        printf("buff_msg_type: %d\n", buff.msg_type);
        printf("buff_service_name: ");
        for (int j = 0; j < MAX_SERVICE_NAME_LEN + 1; j++)
        {
            if(buff.service_name[j] == '\0')
            printf("0");
            else
            printf("%c",buff.service_name[j]);
        }
        printf("\n");
        printf("buff_port: %d\n", buff.port);
        printf("buff_status: %d\n", buff.status);
        printf("\n\n");
        
    }
    for (int i = 0; i < 10; i++)
    {
        req.msg_type = i;
        req.status = i;
        strcpy(req.service_name, "this is the service name" + i);
        req.port = 1000 + i;
        _ptr = encode(&req, &req);

        printf("buff_ptr: %010x\n", &_ptr);
        printf("req: %010x\n", &req);
        printf("req_msg_type: %d\n", req.msg_type);
        printf("req_service_name: ");
        for (int j = 0; j < MAX_SERVICE_NAME_LEN + 1; j++)
        {
            if(req.service_name[j] == '\0')
            printf("0");
            else
            printf("%c",req.service_name[j]);
        }
        printf("\n");
        printf("req_port: %d\n", req.port);
        printf("req_status: %d\n", req.status);
        
        printf("buff: %010x\n", &buff);
        printf("buff_msg_type: %d\n", buff.msg_type);
        printf("buff_service_name: ");
        for (int j = 0; j < MAX_SERVICE_NAME_LEN + 1; j++)
        {
            if(buff.service_name[j] == '\0')
            printf("0");
            else
            printf("%c",buff.service_name[j]);
        }
        printf("\n");
        printf("buff_port: %d\n", buff.port);
        printf("buff_status: %d\n", buff.status);
        printf("\n\n");
    }

    return 0;
}
