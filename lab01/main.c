/***********************************************************
* File Name     : main.c
* Purpose       : 
* Creation Date : 09-26-2017
* Last Modified : Sat 30 Sep 2017 11:17:03 AM PDT
* Created By    : Mark Shanklin 
**********************************************************/
<<<<<<< HEAD
#include <stdio.h>
#include <stdint.h>
#include "nameserver.h"
=======
#include nameserver.h
#include encode.h

>>>>>>> a6d1674f00634a8878ec779d2ebc560a80033997

int main(void)
{
    request_t req;
    memset(req, 0);
    req.msg_type = i;
    req.status = i;
    encode(&req, &req);

    return 0;
}
