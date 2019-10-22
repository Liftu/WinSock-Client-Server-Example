#include "Serveur.h"

Serveur::Serveur()
{
	if (!Sockets::start())
	{
		/*std::ostringstream error;
		error << "Error socket creation [" << Sockets::GetError() << "]";
		throw std::runtime_error(error.str());*/
		std::cout << "Error socket creation [" << this->GetLastError() << "]";
	}
	else
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET)
		{
			/*std::ostringstream error;
			error << "Error socket initialization [" << Sockets::GetError() << "]";
			throw std::runtime_error(error.str());*/
			std::cout << "Error socket initialization [" << this->GetLastError() << "]";
		}
	}
}

Serveur::~Serveur()
{
	Stop();
	Sockets::release();
	for (int i = 0; i < m_clientThreads.size(); i++)
	{
		m_clientThreads[i].join();
	}
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
		std::cout << "Error bind : " << this->GetLastError();
		return false;
	}
	serveur = listen(m_socket, SOMAXCONN);
	if (serveur != 0)
	{
		std::cout << "Error listen : " << this->GetLastError();
		return 0;
	}
	return true;
}

bool Serveur::Stop()
{
	return Sockets::closeSocket(m_socket);
}

bool Serveur::Send(SOCKET socket, std::vector<unsigned char>& data)
{
	unsigned long networkLen = htonl(data.size());

	if (send(socket, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) != sizeof(networkLen))
	{
		//!< Erreur
		return false;
	}

	unsigned long sentSize = 0;
	for (size_t i = 0; i < data.size(); i += 2048)
	{
		sentSize += send(socket, reinterpret_cast<const char*>(data.data() + i), ((data.size() - i) < 2048 ? data.size() % 2048 : 2048), 0);
	}
	if (sentSize != (unsigned long)data.size())
	{
		//!< Erreur
		return false;
	}
	else
		return true;
}

bool Serveur::SendText(SOCKET socket, std::string text)
{
	std::vector<unsigned char> data(text.begin(), text.end());
	return Send(socket, data);
}

bool Serveur::SendFile(SOCKET socket, std::string filePath)
{
	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return false;
	}

	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer;
	file.read((char*)buffer.data(), fileSize);
	file.close();

	return Send(socket, buffer);
}

bool Serveur::Receive(SOCKET socket, std::vector<unsigned char>& buffer)
{
	unsigned long expectedSize;
	int pending = recv(socket, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
	if (pending <= 0 || pending != sizeof(unsigned long))
	{
		//!< Erreur
		return false;
	}
	expectedSize = ntohl(expectedSize);
	buffer.resize(expectedSize);

	unsigned long receivedSize = 0;
	do {
		int ret = recv(socket, reinterpret_cast<char*>(&buffer[receivedSize]), (expectedSize - receivedSize) * sizeof(unsigned char), 0);
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
	
	return (receivedSize == expectedSize);
}

bool Serveur::ReceiveText(SOCKET socket, std::string &text)
{
	std::vector<unsigned char> buffer;
	bool success = Receive(socket, buffer);
	text = std::string((const char*)buffer.data(), buffer.size());
	return success;
}

bool Serveur::ReceiveFile(SOCKET socket, std::string filePath)
{
	std::vector<unsigned char> buffer;
	Receive(socket, buffer);

	std::ofstream file;
	file.open(filePath, std::ios::out | std::ios::app | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write((const char*)(buffer.data()), buffer.size());
	buffer.clear();
	file.close();

	return true;
}

void Serveur::AcceptClients()
{
	while (1)
	{
		sockaddr_in addr = { 0 };
		int addrlen = sizeof(addr);
		SOCKET newClient = accept(m_socket, (SOCKADDR*)(&addr), &addrlen);
		if (newClient != INVALID_SOCKET)
		{
			////////////// Début du thread //////////////
			m_clientThreads.push_back(std::thread(&Serveur::threadClient, this, newClient, addr));
			//std::thread([this, newClient, from]()
			//{
			//	old thread
			//}).detach();
			////////////// Fin du thread //////////////
		}
		else
			break;
	}
}

void Serveur::threadClient(SOCKET client, sockaddr_in addr)
{
	const std::string clientAddress = Sockets::getAddress(addr);
	const unsigned short clientPort = ntohs(addr.sin_port);
	std::cout << "Connection of " << clientAddress.c_str() << ":" << clientPort << std::endl;
	bool connected = true;
	while (1)
	{
		/////// Début de la boucle d'événement du thread ///////
		/////// Receive request ///////
		ReceiveFile(client, "tata.txt");

		std::vector<unsigned char> buffer;
		if (!Receive(client, buffer))
		{
			std::cout << "[" << clientAddress << ":" << clientPort << "] : ERROR while receiving data." << std::endl;
			Sockets::closeSocket(client);
			break;
		}

		/////// Process request ///////
		std::string message((const char*)buffer.data(), buffer.size());
		std::string reponse;
		if (message == "exit")
		{
			reponse = "Ok good bye !";
		}
		else if (message == "Hello world !")
			reponse = "Welcome man ! ";
		else if (message == "Bye !")
			reponse = "Take care !";
		else
			reponse = "Hey !";
		std::cout << "[" << clientAddress << ":" << clientPort << "] => " << message << std::endl;

		/////// Send answer ///////
		if (!SendText(client, reponse))
		{
			//!< Erreur
			std::cout << "[" << clientAddress << ":" << clientPort << "] : ERROR sending answer." << std::endl;
			Sockets::closeSocket(client);
			break;
		}

		//////// Has exited ? ////////
		if (message == "exit")
		{
			std::cout << "[" << clientAddress << ":" << clientPort << "] Disconnected. " << std::endl;
			closesocket(client);
			break;
		}
		/////// Fin de la boucle d'événement du thread ///////
	}
}
