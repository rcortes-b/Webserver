# include "../includes/SimpleSocket.hpp"
# include "../includes/Response.hpp"
# include "../includes/ConfigFile.hpp"

SimpleSocket::SimpleSocket()
{
	this->serverSocket = -1;
	this->addrinfo = NULL;
}

SimpleSocket::SimpleSocket(std::string port)
{
	(void)port; //what is this
}

void	SimpleSocket::setSocket(std::string port, std::string host)
{
	t_addrinfo hints;
	int enable = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &this->addrinfo);
	if (status != 0)
	{
		std::cout << gai_strerror(status) << '\n';
		throw NoMsgError();
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
	std::cout << "ACCEPTING CONNECTION..." << '\n';
	int clientSocket = accept(this->serverSocket, reinterpret_cast<t_sockaddr *>(&clientadrr), reinterpret_cast<socklen_t *>(&adrrin_size));
	std::cout << "clientSocket = " << clientSocket << '\n';
	if (clientSocket < 0)
		throw SysError();
	std::cout << "CONNECTION ACCEPTED!" << '\n';
	return (clientSocket);
}

int	SimpleSocket::getServerSocket(void) const
{
	return (this->serverSocket);
}

void SimpleSocket::clearData(void)
{
	std::cout << "CLEANING SOCKET" << '\n';
	if (this->addrinfo != NULL)
		freeaddrinfo(this->addrinfo);
	if (this->serverSocket != -1)
		close(this->serverSocket);
}
