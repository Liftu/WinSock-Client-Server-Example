#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ws2tcpip.h>
#include "Sockets.h"

class Client
{
public:
	Client();
	~Client();
	bool Connect(const std::string& ip, unsigned short port);
	bool Disconnect();
	int GetLastError() { return Sockets::getError(); }
	bool Send(std::vector<unsigned char>& data);
	bool SendText(std::string Text);
	bool SendFile(std::string filePath);
	bool Receive(std::vector<unsigned char>& buffer);
	bool ReceiveText(std::string &text);
	bool ReceiveFile(std::string filePath);

private:
	SOCKET m_socket;
};

#endif // CLIENT_H
