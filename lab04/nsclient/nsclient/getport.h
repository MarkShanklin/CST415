#pragma once

using namespace System;

using namespace System::Net;
using namespace System::Net::Sockets;

#include "nameserver.h"

public ref class Nameserver
{
public:
	Nameserver();
	int define_port(const char *name, int port);
	int request_port(const char *name);
	int close_port(const char *name, int port);
	int lookup_port(const char *name);

protected:
	int m_service_port;
	String^ m_service_name;
	UdpClient^ m_nameserver;

	int process_request(const char *name, int type, int port);

};

