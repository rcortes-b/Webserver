# include "../includes/SimpleSocket.hpp"
# include "../includes/Response.hpp"
# include "../includes/ConfigFile.hpp"
# include <stdlib.h>

SimpleSocket::SimpleSocket()
{
	this->serverSocket = -1;
	this->addrinfo = NULL;
}

int	SimpleSocket::setSocket(std::string port, std::string host)
{
	t_addrinfo hints;
	int enable = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &this->addrinfo);
	if (status != 0)
	{
		std::cerr << gai_strerror(status) << '\n';
		return (1);
	}

	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		SysError();
	
	int flags = fcntl(this->serverSocket, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if (fcntl(this->serverSocket, F_SETFL, flags) < 0)
		SysError();

	if (this->serverSocket < 0)
		SysError();

	if (bind(this->serverSocket,this->addrinfo->ai_addr, this->addrinfo->ai_addrlen) < 0)
		SysError();

	if (listen(this->serverSocket, MAX_CONNECTIONS) < 0)
		SysError();

	return (0);
}

void SimpleSocket::setServer(ServerConfig *server)
{
	this->server = server;
}

ServerConfig *SimpleSocket::getServer(void) const
{
	return (this->server);
}

SimpleSocket::SimpleSocket(const SimpleSocket &other)
{
	this->serverSocket = other.serverSocket;
	this->addrinfo = other.addrinfo;
	this->server = other.server;
}

SimpleSocket	&SimpleSocket::operator=(const SimpleSocket &other)
{
	this->serverSocket = other.serverSocket;
	this->addrinfo = other.addrinfo;
	this->server = other.server;

	return (*this);
}

SimpleSocket::~SimpleSocket()
{
}

int	SimpleSocket::acceptConnection(void)
{
	int	adrrin_size = sizeof(t_sockaddr_in);
	t_sockaddr_in clientadrr;
	int clientSocket = accept(this->serverSocket, reinterpret_cast<t_sockaddr *>(&clientadrr), reinterpret_cast<socklen_t *>(&adrrin_size));

	if (clientSocket < 0)
		throw SysError();
	
	return (clientSocket);
}

int	SimpleSocket::getServerSocket(void) const
{
	return (this->serverSocket);
}

void SimpleSocket::clearData(void)
{
	if (this->addrinfo != NULL)
		freeaddrinfo(this->addrinfo);
	if (this->serverSocket != -1)
		close(this->serverSocket);
}
