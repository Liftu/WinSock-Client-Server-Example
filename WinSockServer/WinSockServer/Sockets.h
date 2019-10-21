#ifndef SOCKETS_H
#define SOCKETS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


namespace Sockets
{
	bool start();
	void release();
	int getError();
	bool closeSocket(SOCKET socket);
	std::string getAddress(const sockaddr_in& addr);
}

#endif //SOCKETS_H
