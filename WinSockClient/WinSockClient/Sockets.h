#ifndef SOCKETS_H
#define SOCKETS_H

#include <winsock2.h>

namespace Sockets
{
	bool Start();
	void Release();
	int GetError();
	bool CloseSocket(SOCKET socket);
}

#endif //SOCKETS_H
