#include "Client.h"

Client::Client()
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

Client::~Client()
{
	Disconnect();
	Sockets::Release();
}

bool Client::Connect(const std::string& ipaddress, unsigned short port)
{
	sockaddr_in server;
	InetPtonA(AF_INET, ipaddress.c_str(), &(server.sin_addr));
	//server.sin_addr.s_addr = inet_addr(ipaddress.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	return connect(m_socket, (const sockaddr*)&server, sizeof(server)) == 0;
}

bool Client::Disconnect()
{
	return Sockets::CloseSocket(m_socket);
}

bool Client::Send(std::string data)
{
	const unsigned char* dataChar = (unsigned char*)data.c_str();
	unsigned long networkLen = htonl(data.size());

	if (send(m_socket, reinterpret_cast<const char*>(&networkLen), sizeof(networkLen), 0) != sizeof(networkLen))
		return false;

	unsigned long sentSize = 0;
	for (size_t i = 0; i < data.size(); i += 2048)
	{
		sentSize += send(m_socket, reinterpret_cast<const char*>(dataChar+i), ((data.size() - i) < 2048 ? data.size() % 2048 : 2048), 0);
	}
		
	return (sentSize == (unsigned long)data.size());
}

bool Client::Receive(std::vector<unsigned char>& buffer)
{
	unsigned long expectedSize;
	int pending = recv(m_socket, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
	if (pending <= 0 || pending != sizeof(unsigned long))
	{
		//!< Erreur
		return false;
	}
	expectedSize = ntohl(expectedSize);
	buffer.resize(expectedSize);

	unsigned long receivedSize = 0;
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
