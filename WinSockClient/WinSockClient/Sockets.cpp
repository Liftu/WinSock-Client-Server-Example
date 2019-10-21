#include "Sockets.h"

namespace Sockets
{
	bool start()
	{
		WSAData wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}
	void release()
	{
		WSACleanup();
	}
	int getError()
	{
		return WSAGetLastError();
	}
	bool closeSocket(SOCKET s)
	{
		return closesocket(s) == 0;
	}
}
