#include <iostream>
#include "Client.h"
#include <windows.h>

using namespace std;

int main()
{
	std::string ip = "127.0.0.1";
	unsigned short port = 5050;
	Client client;
	if (!client.Connect(ip, port))
	{
		std::cout << "Error connection : " << client.GetLastError() << std::endl;
		return 0;
	}
	std::cout << "Client connected on : " << ip << ":" << port << std::endl;

	client.SendFile("toto.txt");

	std::string message;
	while (message != "exit")
	{
		std::cout << "Message to send (\"exit\" to quit): ";
		std::getline(std::cin, message);

		if (!client.SendText(message))
		{
			std::cout << "Error while sending the message to the server : \"" << message << "\". Error : " << client.GetLastError() << std::endl;
		}
		else
		{
			std::vector<unsigned char> buffer;
			if (!client.Receive(buffer))
			{
				std::cout << "Error while receiving the server response : " << client.GetLastError() << std::endl;
			}
			else
			{
				std::string reponse((const char*)buffer.data(), buffer.size());
				std::cout << "Reponse du serveur : " << reponse << std::endl;
			}
		}
	}
	client.Disconnect();
	//system("PAUSE");
	return 0;
}
