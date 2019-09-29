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
	int GetLastError() { return Sockets::GetError(); }
	bool Send(std::string data);
	bool Receive(std::vector<unsigned char>& buffer);
	void AcceptClients();
private:
	SOCKET m_socket;
	sockaddr_in m_adresse;

};

#endif // SERVEUR_H
