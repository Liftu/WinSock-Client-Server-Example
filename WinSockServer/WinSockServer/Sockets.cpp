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
	std::string getAddress(const sockaddr_in& addr)
	{
		//return inet_ntoa(addr.sin_addr);
		LPSTR ip = (LPSTR)malloc(16);
		InetNtopA(AF_INET, &addr.sin_addr, ip, 16);
		return std::string(ip);
	}
}
