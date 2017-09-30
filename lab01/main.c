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

int main(void)
{
    request_t req;
    memset(req, 0, sizeof(request_t));
    req.msg_type = i;
    req.status = i;
    
    encode(&req, &req);

    return 0;
}
