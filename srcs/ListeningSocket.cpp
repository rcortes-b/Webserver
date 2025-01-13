# include "../includes/ListeningSocket.hpp"

ListeningSocket::ListeningSocket()
{
	this->bufferStr = "";
	this->server = NULL;
	this->bufferChar = NULL;
	this->bufferCharSize = 0;
	this->bytesHeaderRead = 0;

	this->firstBody = NULL;
	this->firstBodySize = 0;
	this->token = "";
}

ListeningSocket &ListeningSocket::operator=(const ListeningSocket &other)
{
	
	this->bufferStr = other.bufferStr;
	if (this->server && other.server)
		*this->server = *other.server;
	else
		this->server = NULL;
	this->bufferChar = other.bufferChar;
	this->bufferCharSize = other.bufferCharSize;

	this->firstBody = other.firstBody;
	this->firstBodySize = other.firstBodySize;
	this->token = other.token;
	this->bytesHeaderRead = other.bytesHeaderRead;

	return (*this);
}

ListeningSocket::ListeningSocket(const ListeningSocket &other)
{
	*this = other;
}

ListeningSocket::~ListeningSocket()
{
	if (this->bufferChar)
		delete[] this->bufferChar;
	if (this->firstBody)
		delete[] this->firstBody;
}

int	ListeningSocket::readPetition(int clientFd)
{
	std::cout << "BEFORE READ PETITION" << '\n';
	if (this->bufferChar == NULL)
	{
		int status = this->readHeader(clientFd);
		if (status == 1)
			return (1);
		else if (status == 2)
			return (2);
	}
	if (this->token != "")
	{
		int status = this->readBody(clientFd);
		if (status == 0)
			return (0);
		else if (status == 1)
			return (1);
		else if (status == 2)
			return (2);
	}

	std::cout << "AFTER READ PETICION" << '\n';
	return (0);
}

int ListeningSocket::readHeader(int clientFd)
{
	char buffer[MAX_BUFFER_SIZE + 1];
	ssize_t	bytesRead;
	std::memset(buffer, 0, MAX_BUFFER_SIZE + 1);

	bytesRead = recv(clientFd, buffer, MAX_BUFFER_SIZE, 0);
	std::cout << "BYTES READ == " << bytesRead << '\n';
	if (bytesRead < 0) //ERROR
		return (1);
	else if (bytesRead == 0) // CONNECTION CLOSED BY CLIENT
		return (2);
	this->bytesHeaderRead += bytesRead;
	std::cout << "bytesRead: " << bytesRead << '\n';
	std::string str_buffer(buffer);
	this->bufferStr.append(str_buffer);
	//int token = -1; what is this

	if (this->bufferStr.find("\r\n\r\n") != std::string::npos)
	{
		ssize_t start = this->bufferStr.find("\r\n\r\n") + 4;
		this->token = "\r\n\r\n";
		this->firstBody = &buffer[start];
		this->firstBodySize = this->bytesHeaderRead - start;
		this->firstBody = new char[this->firstBodySize];
		std::memcpy(this->firstBody, &buffer[start], this->firstBodySize);
	}
	else if (this->bufferStr.find("\n\n") != std::string::npos)
	{
		ssize_t start = this->bufferStr.find("\n\n") + 2;
		this->token = "\n\n";
		this->firstBody = &buffer[start];
		this->firstBodySize = this->bytesHeaderRead - start;
		this->firstBody = new char[this->firstBodySize];
		std::memcpy(this->firstBody, &buffer[start], this->firstBodySize);
	}

	return (0);
}

int ListeningSocket::readBody(int clientFd)
{
	size_t token_pos = this->bufferStr.find(this->token);
	std::string header = this->bufferStr.substr(0, token_pos + this->token.length());

	ssize_t content_len = 0;
	size_t start = header.find("Content-Length:");
	if (start != std::string::npos)
	{
		size_t end;
		start = start + 15;
		if ((end = header.find("\r\n", start)) == std::string::npos && (end = header.find("\n", start)) == std::string::npos)
			return (1);
		content_len = strToulNum(header.substr(start, end - start));
		if (this->bufferChar == NULL)
		{
			this->bufferChar = new char[this->firstBodySize + content_len];
			this->bufferCharSize += this->firstBodySize;
			std::memcpy(this->bufferChar, firstBody, firstBodySize);
		}
		while (this->bufferCharSize < content_len)
		{
			ssize_t readedBytes = recv(clientFd, &this->bufferChar[this->bufferCharSize], 1024, 0);	
			//std::cout << "bodyBufferSize: " << bufferCharSize << "    readedBytes: " <<  readedBytes << "   content_len: " << content_len << "  bodySize: " << firstBodySize << '\n';
			if (readedBytes == 0)
				break;
			else if (readedBytes < 0)
				return (0);

			this->bufferCharSize += readedBytes;
		}
	}
	
	this->bufferStr = header;
	std::cout << "PETICION:\n" << header;
	if (this->bufferChar)
		std::cout << "asds: " <<this->bufferChar;
	std::cout << '\n';

	handlePetition(this->bufferStr, this->bufferChar, this->bufferCharSize, clientFd, *(this->server));

	return (2);
}

void ListeningSocket::setServer(ServerConfig *server)
{
	this->server = server;
}

ServerConfig *ListeningSocket::getServer(void) const
{
	return (this->server);
}

void ListeningSocket::setBufferStr(std::string new_bufferStr)
{
	this->bufferStr = new_bufferStr;
}

std::string &ListeningSocket::getBufferStr(void)
{
	return (this->bufferStr);
}

char *ListeningSocket::getBufferChar(void)
{
	return (this->bufferChar);
}

ssize_t &ListeningSocket::getBufferCharSize(void)
{
	return (this->bufferCharSize);
}