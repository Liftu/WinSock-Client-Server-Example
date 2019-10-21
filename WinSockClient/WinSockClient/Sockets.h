#ifndef SOCKETS_H
#define SOCKETS_H

#include <winsock2.h>

namespace Sockets
{
	bool start();
	void release();
	int getError();
	bool closeSocket(SOCKET socket);
}

#endif //SOCKETS_H
