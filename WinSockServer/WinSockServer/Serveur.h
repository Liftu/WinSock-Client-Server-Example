#ifndef SERVEUR_H
#define SERVEUR_H

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include "Sockets.h"
#include <windows.h>

class Serveur
{
public:
	Serveur();
	~Serveur();
	bool Start(unsigned short port);
	bool Stop();
	int GetLastError() { return Sockets::getError(); }
	bool Send(SOCKET socket, std::vector<unsigned char>& data);
	bool SendText(SOCKET socket, std::string text);
	bool Receive(SOCKET socket, std::vector<unsigned char>& buffer);
	void AcceptClients();

private:
	void threadClient(SOCKET client, sockaddr_in addr);

	SOCKET m_socket;
	sockaddr_in m_adresse;
	std::vector<std::thread> m_clientThreads;
};

#endif // SERVEUR_H
