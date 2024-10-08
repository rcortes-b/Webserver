# include "../includes/SimpleSocket.hpp"

SimpleSocket::SimpleSocket()
{
	this->serverSocket = -1;
	this->addrinfo = nullptr;
}

SimpleSocket::SimpleSocket(std::string port)
{
}

void	SimpleSocket::setSocket(std::string port)
{
	t_addrinfo hints;
	int enable = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo(IP_HOST, port.c_str(), &hints, &this->addrinfo);
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

SimpleSocket::SimpleSocket(const SimpleSocket &other)
{
	this->serverSocket = other.serverSocket;
	this->addrinfo = other.addrinfo;
}

SimpleSocket	&SimpleSocket::operator=(const SimpleSocket &other)
{
	this->serverSocket = other.serverSocket;
	this->addrinfo = other.addrinfo;

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

int	SimpleSocket::readPetition(int clientFd)
{
	std::cout << "READ PETITION" << '\n';
	char buffer[MAX_BUFFER_SIZE];
	size_t	msgSize = 0;
	size_t	bytesRead;

	while (true)
	{
		bytesRead = recv(clientFd, buffer + msgSize, MAX_BUFFER_SIZE, 0);
		msgSize += bytesRead;
		if (bytesRead < 0)
		{
			close(clientFd);
			return (1);
		}
		// TEMPORAL LO DEL TERMINAR CUANDO TERMINA EL MENSAJE ES POR KEEP-ALIVE DE LA PETICION
		if (bytesRead == 0 || (buffer[msgSize - 4] == '\r' && buffer[msgSize - 3] == '\n' && buffer[msgSize - 2] == '\r' && buffer[msgSize - 1] == '\n'))
			break;
	}
	buffer[msgSize] = '\0';
	// TEMPORAL AQUI LLAMAR FUNCION PARA GESTIONAR PETICIONES
	std::cout << "PETICION:\n" << buffer << '\n';
	// FINS AQUI
	std::cout << "AFTER PETICION" << '\n';
	close(clientFd);
	return (0);
}

int	SimpleSocket::getServerSocket(void) const
{
	return (this->serverSocket);
}

void SimpleSocket::clearData(void)
{
	if (this->addrinfo != nullptr)
		freeaddrinfo(this->addrinfo);
	if (this->serverSocket != -1)
		close(this->serverSocket);
}