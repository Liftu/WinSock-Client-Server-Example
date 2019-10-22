#ifndef SERVEUR_H
#define SERVEUR_H

#include <vector>
#include <string>
#include <fstream>
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
	bool SendFile(SOCKET socket, std::string filePath);
	bool Receive(SOCKET socket, std::vector<unsigned char>& buffer);
	bool ReceiveText(SOCKET socket, std::string &text);
	bool ReceiveFile(SOCKET socket, std::string filePath);
	void AcceptClients();

private:
	void threadClient(SOCKET client, sockaddr_in addr);

	SOCKET m_socket;
	sockaddr_in m_adresse;
	std::vector<std::thread> m_clientThreads;
};

#endif // SERVEUR_H
