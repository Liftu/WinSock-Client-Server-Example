#include "Serveur.h"

Serveur::Serveur()
{
	if (!Sockets::Start())
	{
		/*std::ostringstream error;
		error << "Erreur creation socket [" << Sockets::GetError() << "]";
		throw std::runtime_error(error.str());*/
		std::cout << "Erreur creation socket [" << this->GetLastError() << "]";
	}
	else
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET)
		{
			/*std::ostringstream error;
			error << "Erreur initialisation socket [" << Sockets::GetError() << "]";
			throw std::runtime_error(error.str());*/
			std::cout << "Erreur initialisation socket [" << this->GetLastError() << "]";
		}
	}
}

Serveur::~Serveur()
{
	Stop();
	Sockets::Release();
}

bool Serveur::Start(unsigned short port)
{
	sockaddr_in m_adresse;
	m_adresse.sin_addr.s_addr = INADDR_ANY;
	m_adresse.sin_family = AF_INET;
	m_adresse.sin_port = htons(port);

	int serveur = bind(m_socket, (sockaddr*)&m_adresse, sizeof(m_adresse));
	if (serveur != 0)
	{
		std::cout << "Erreur bind : " << this->GetLastError();
		return false;
	}
	serveur = listen(m_socket, SOMAXCONN);
	if (serveur != 0)
	{
		std::cout << "Erreur listen : " << this->GetLastError();
		return 0;
	}
	return true;
}

bool Serveur::Stop()
{
	return Sockets::CloseSocket(m_socket);
}

bool Serveur::Send(std::string data)
{
	const unsigned char* dataChar = (unsigned char*)data.c_str();
	short networkLen = htons(data.size());
	return send(m_socket, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) == sizeof(networkLen)
		&& send(m_socket, reinterpret_cast<const char*>(dataChar), data.size(), 0) == (int)data.size();
}

bool Serveur::Receive(std::vector<unsigned char>& buffer)
{
	unsigned short expectedSize;
	int pending = recv(m_socket, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
	if (pending <= 0 || pending != sizeof(unsigned short))
	{
		//!< Erreur
		return false;
	}

	expectedSize = ntohs(expectedSize);
	buffer.resize(expectedSize);
	int receivedSize = 0;
	do {
		int ret = recv(m_socket, reinterpret_cast<char*>(&buffer[receivedSize]), (expectedSize - receivedSize) * sizeof(unsigned char), 0);
		if (ret <= 0)
		{
			//!< Erreur
			buffer.clear();
			return false;
		}
		else
		{
			receivedSize += ret;
		}
	} while (receivedSize < expectedSize);
	return true;
}

void Serveur::AcceptClients()
{
	while (1)
	{
		sockaddr_in from = { 0 };
		int addrlen = sizeof(from);
		SOCKET newClient = accept(m_socket, (SOCKADDR*)(&from), &addrlen);
		if (newClient != INVALID_SOCKET)
		{
			////////////// Début du thread //////////////
			std::thread([newClient, from]()
			{
				const std::string clientAddress = Sockets::GetAddress(from);
				const unsigned short clientPort = ntohs(from.sin_port);
				std::cout << "Connexion de " << clientAddress.c_str() << ":" << clientPort << std::endl;
				bool connected = true;
				while (1)
				{
					/////// Début de la boucle d'événement du thread ///////
					/////// Receive request ///////
					std::vector<unsigned char> buffer;
					unsigned short expectedSize;
					int pending = recv(newClient, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
					if (pending <= 0 || pending != sizeof(unsigned short))
					{
						//!< Erreur
						std::cout << "[" << clientAddress << ":" << clientPort << "] Connection closed." << std::endl;
						Sockets::CloseSocket(newClient);
						break;
					}

					expectedSize = ntohs(expectedSize);
					buffer.resize(expectedSize);
					int receivedSize = 0;

					do {
						int ret = recv(newClient, reinterpret_cast<char*>(&buffer[receivedSize]), (expectedSize - receivedSize) * sizeof(unsigned char), 0);
						if (ret <= 0)
						{
							//!< Erreur
							//buffer.clear();
							std::cout << "[" << clientAddress << ":" << clientPort << "] : ERROR while receiving data." << std::endl;
							Sockets::CloseSocket(newClient);
							break;
						}
						else
						{
							receivedSize += ret;
						}
					} while (receivedSize < expectedSize);

					/////// Process request ///////
					std::string message((const char*)buffer.data(), buffer.size());
					std::string reponse;
					if (message == "exit")
					{
						std::cout << "[" << clientAddress << ":" << clientPort << "] Disconnected. " << std::endl;
						closesocket(newClient);
						break;
					}
					else if (message == "Hello world !")
						reponse = "Welcome man ! ";
					else if (message == "Bye !")
						reponse = "Take care !";
					else
						reponse = "Hey !";
					std::cout << "[" << clientAddress << ":" << clientPort << "] => " << message << std::endl;

					/////// Send answer ///////
					const unsigned char* dataChar = (unsigned char*)reponse.c_str();
					short networkLen = htons(reponse.size());
					if (!(send(newClient, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) == sizeof(networkLen)
						&& send(newClient, reinterpret_cast<const char*>(dataChar), reponse.size(), 0) == (int)reponse.size()))
					{
						//!< Erreur
						//buffer.clear();
						std::cout << "[" << clientAddress << ":" << clientPort << "] : ERROR sending answer." << std::endl;
						Sockets::CloseSocket(newClient);
						break;
					}
					/////// Fin de la boucle d'événement du thread ///////
				}
			}).detach();
			////////////// Fin du thread //////////////
		}
		else
			break;
	}
}
