# include "../includes/ListeningSocket.hpp"

ListeningSocket::ListeningSocket()
{
	this->buffer = "";
	this->server = NULL;
}

ListeningSocket &ListeningSocket::operator=(const ListeningSocket &other)
{
	this->buffer = other.buffer;
	if (this->server && other.server)
		*this->server = *other.server;
	else
		this->server = NULL;

	return (*this);
}

ListeningSocket::ListeningSocket(const ListeningSocket &other)
{
	*this = other;
}

ListeningSocket::~ListeningSocket()
{
}

void ListeningSocket::setServer(ServerConfig *server)
{
	this->server = server;
}

ServerConfig *ListeningSocket::getServer(void) const
{
	return (this->server);
}

void ListeningSocket::setBuffer(std::string new_buffer)
{
	this->buffer = new_buffer;
}

std::string &ListeningSocket::getBuffer(void)
{
	return (this->buffer);
}