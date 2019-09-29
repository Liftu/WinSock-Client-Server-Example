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
		std::cout << "Erreur connection : " << client.GetLastError() << std::endl;
		return 0;
	}
	std::cout << "Client connecte sur : " << ip << ":" << port << std::endl;

	std::string message;
	while (message != "exit")
	{
		std::cout << "Message to send (\"exit\" to quit): ";
		std::getline(std::cin, message);

		if (!client.Send(message))
		{
			std::cout << "Erreur lors de l'envoi du message au serveur : \"" << message << "\". Erreur : " << client.GetLastError() << std::endl;
		}
		else
		{
			std::vector<unsigned char> buffer;
			if (!client.Receive(buffer))
			{
				std::cout << "Erreur lors de la reception de la reponse du serveur : " << client.GetLastError() << std::endl;
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
