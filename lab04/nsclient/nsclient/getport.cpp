#include "stdafx.h"
#include <Winsock2.h>

#include "getport.h"
#include "nameserver.h"

#include "stdafx.h"


Nameserver::Nameserver()
{
	m_service_port = 50100;
	m_service_name = gcnew String("unix.cset.oit.edu");

	m_nameserver = gcnew UdpClient;
}
int Nameserver::define_port(const char *name, int port)
{
	return process_request(name, DEFINE_PORT, port);
}
int Nameserver::request_port(const char *name)
{
	return process_request(name, REQUEST_PORT, 0);
}
int Nameserver::close_port(const char *name, int port)
{
	return process_request(name, CLOSE_PORT, port);
}
int Nameserver::lookup_port(const char *name)
{
	return process_request(name, LOOKUP_PORT, 0);
}

int Nameserver::process_request(const char *name, int type, int port)
{
	request_t request;

	request.msg_type = type;
	strcpy(request.service_name, name);
	request.port = htons(port);
	request.status = SUCCESS;

	printf("Sending %d %s %d\n", request.msg_type, request.service_name, request.port);

	array<Byte>^byteMsg = gcnew array<Byte>(sizeof(request));
	pin_ptr<Byte>p = &byteMsg[0];
	Byte *np = p;
	memcpy(np, &request, sizeof(request));

	m_nameserver->Send(byteMsg, sizeof(request), m_service_name, m_service_port);
	Console::WriteLine("Sent message");

	IPEndPoint^ RemoteIpEndPoint = gcnew IPEndPoint(IPAddress::Any, 0);
	array<Byte>^recvBytes = m_nameserver->Receive(RemoteIpEndPoint);
	pin_ptr<Byte>p2 = &recvBytes[0];
	Byte *np2 = p2;
	memcpy(&request, np2, sizeof(request));

	printf("Received %d %s %d %d\n", request.msg_type, request.service_name, request.port, request.status);
	if (request.status != SUCCESS)
			printf("Error %d on request\n", request.status);
	
	return ntohs(request.port);
}