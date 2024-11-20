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

int	SimpleSocket::readPetition(int clientFd, std::string &petition, ServerConfig &server)
{
	std::cout << "BEFORE READ PETITION" << '\n';
	char buffer[MAX_BUFFER_SIZE + 1];
	int	bytesRead;
	std::memset(buffer, 0, MAX_BUFFER_SIZE + 1);

	// SI VOLEM FER MULTIPLEXING (HTTP/2) PODIEM FER UN FILL CADA COP QUE LLEGIM UNA PETICIO I QUE EL PARE ES QUEDI ESCOLTANT
	bytesRead = recv(clientFd, buffer, MAX_BUFFER_SIZE, 0);
	std::cout << "BYTES READ == " << bytesRead << '\n';
	if (bytesRead < 0) //ERROR
		return (1);
	else if (bytesRead == 0) // CONNECTION CLOSED BY CLIENT
		return (2);
	std::string str_buffer(buffer);
	petition.append(str_buffer);
	//int token = -1; what is this

	if (petition.find("\r\n\r\n") != std::string::npos)
	{
		//JUST A TEST THAT WORKS
		// std::cout << "FOUND in " << start << '\n';
		// std::cout << "char *test: " << &buffer[start] << " testSize: " << MAX_BUFFER_SIZE - start << '\n';
		// std::ofstream pathFile("./www/web2/uploads/testFINAL.jpg", std::ios::binary | std::ios::trunc);
		// pathFile.write(&buffer[start], MAX_BUFFER_SIZE - start);
		// pathFile.close();
		//FINS AQUI
		ssize_t start = petition.find("\r\n\r\n") + 4;
		std::cout << "MAX_BUFFERSIZE: " << MAX_BUFFER_SIZE << "   -   start: " << buffer[start] << '\n';
		if (SimpleSocket::readBody(petition, "\r\n\r\n", clientFd, server, &buffer[start], bytesRead - start))
			return (1);
		else
			return (2);
	}
	else if (petition.find("\n\n") != std::string::npos)
	{
		ssize_t start = petition.find("\n\n") + 2;
		if (SimpleSocket::readBody(petition, "\n\n", clientFd, server, &buffer[start], MAX_BUFFER_SIZE - start))
			return (1);
		else
			return (2);
	}

	std::cout << "AFTER READ PETICION" << '\n';
	return (0);
}

int SimpleSocket::readBody(std::string &petition, std::string token, int clientFd, ServerConfig &server, char *bodyContent, ssize_t bodySize)
{
	size_t token_pos = petition.find(token);
	std::string header = petition.substr(0, token_pos + token.length());

	u_long content_len = 0;
	char *buffer = NULL;
	size_t start = header.find("Content-Length:");
	if (start != std::string::npos)
	{
		size_t end;
		start = start + 15;
		if ((end = header.find("\r\n")) == std::string::npos && (end = header.find("\n")) == std::string::npos)
			return (1);
		std::cout << "bodySize: " << bodySize << "    content_len: " << strToulNum(header.substr(start, end - start)) << '\n';
		content_len = strToulNum(header.substr(start, end - start)) - bodySize;
		buffer = new char[bodySize + content_len];
		std::memcpy(buffer, bodyContent, bodySize);
		u_long totalBytes = 0;
		while (totalBytes < content_len)
		{
			ssize_t readedBytes = recv(clientFd, &buffer[bodySize + totalBytes], content_len, 0);	
			if (readedBytes <= 0)
				break;

			totalBytes += readedBytes;
		}
	}
	
	petition = header;
	std::cout << "PETICION:\n" << header;
	if (buffer)
		std::cout << buffer;
	std::cout << '\n';

	handlePetition(petition, buffer, bodySize + content_len, clientFd, server);

	if (buffer)
		delete[] buffer;
	return (0);
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
