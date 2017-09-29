/***********************************************************
* File Name     : encode.c
* Purpose       : encode decode and validate data
* Creation Date : 09-26-2017
* Last Modified : Wed 27 Sep 2017 08:33:26 PM PDT
* Created By    : Mark Shanklin 
***********************************************************/
#include nameserver.h

void *encode(request_t *request, void *buff)
{
    if (request == buff)
    {
        memmove(request->service_name, ((request *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
    }
    else
    {
        memcpy(request->service_name, ((request *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
    }
    ((request_t *)buff)->serivce_name[MAX_SERVICE_NAME_LEN] = NULL; //make last item in string NULL
    int length = request->service_name.length;                      //find the length up to the first NULL
    memset(buff + length, NULL, MAX_SERVICE_NAME_LEN - length);     //NULL fill from first NULL to end
    ((request_t *)buff)->msg_type = request->msg_type;              //copy msg type only one byte no need to network order
    ((request_t *)buff)->port = htons(request->port);               //copy port using network byte order
    ((request_t *)buff)->status = request->status;                  //copy status only one byte no need to network order
    return buff;
};

int is_invalid(request_t *request)
{
    int ret_val = 0;
    int valid = 1;

    //do validation stuff edit valid based on errors
    //request->msg_type is valid int8_t
    //request->service_name is valid char [MAX_SERVICE_NAME_LEN +1]
    //request->port is valid uint16_

    //2-whatever number for error codes.

    if (valid == 1)
    {
        ret_val = 0;
    }
    else
        ret_val = valid;
    return ret_val;
};

request_t *decode(void *buff, request_t *decoded)
{
    if (is_invalid((request_t *)buff))
        return;
    if (request == buff)
    {
        memmove(decoded->service_name, ((request *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
    }
    else
    {
        memcpy(decoded->service_name, ((request *)buff)->service_name, MAX_SERVICE_NAME_LEN); //(dest,src,length)
    }
    decoded->msg_type = ((request_t *)buff)->msg_type; //copy msg type only one byte no need to network order
    decoded->port = nstoh(((request_t *)buff)->port);  //copy port using host byte order
    ((request_t *)buff)->status = request->status;     //copy status only one byte no need to host order
    return buff;
};
