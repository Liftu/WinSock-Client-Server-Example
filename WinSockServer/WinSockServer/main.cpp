#include <iostream>
#include "Serveur.h"
#include <windows.h>

using namespace std;

int main()
{
	unsigned short port = 5050;
	Serveur serveur;
	if (!serveur.Start(port))
	{
		std::cout << "Error starting server : " << serveur.GetLastError() << std::endl;
		return 0;
	}
	std::cout << "Server running on port : " << port << std::endl;
	serveur.AcceptClients();
	serveur.Stop();
	return 0;
}
