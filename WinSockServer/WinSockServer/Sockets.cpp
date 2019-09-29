#include "Sockets.h"

namespace Sockets
{
	bool Start()
	{
		WSAData wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}
	void Release()
	{
		WSACleanup();
	}
	int GetError()
	{
		return WSAGetLastError();
	}
	bool CloseSocket(SOCKET s)
	{
		return closesocket(s) == 0;
	}
	std::string GetAddress(const sockaddr_in& addr)
	{
		//return inet_ntoa(addr.sin_addr);
		LPSTR ip = (LPSTR)malloc(16);
		InetNtopA(AF_INET, &addr.sin_addr, ip, 16);
		return std::string(ip);
	}
}
