// nsclient.cpp : main project file.

#include "stdafx.h"

#include "getport.h"

int main(array<System::String ^> ^args)
{
	Nameserver server;

	int port = server.define_port("pwh_server_1", 51001);
	printf("Got port %d\n", port);

	port = server.request_port("pwh_server_2");
	printf("Got port %d\n", port);

	int port2 = server.lookup_port("pwh_server_1");
	printf("Found port %d\n", port2);

	port2 = server.lookup_port("pwh_server_2");
	printf("Found port %d\n", port2);

	server.close_port("pwh_server_1", 51001);
	printf("Closed port %d\n", port);

	server.close_port("pwh_server_2", port);
	printf("Closed port %d\n", port);

	return 0;
}


