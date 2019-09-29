#ifndef SOCKETS_H
#define SOCKETS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


namespace Sockets
{
	bool Start();
	void Release();
	int GetError();
	bool CloseSocket(SOCKET socket);
	std::string GetAddress(const sockaddr_in& addr);
}

#endif //SOCKETS_H
