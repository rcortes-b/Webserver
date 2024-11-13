# include "../includes/Petition.hpp"

Petition::Petition()
{
	this->type = "text/plain";
}

Petition::Petition(const Petition &other)
{
	this->method = other.method;
	this->path = other.path;
	this->protocol = other.protocol;
	this->type = other.type;
	this->headers = other.headers;
}

Petition &Petition::operator=(const Petition &other)
{
	if (this != &other)
	{
		this->method = other.method;
		this->path = other.path;
		this->protocol = other.protocol;
		this->type = other.type;
		this->headers = other.headers;
	}
	return (*this);
}

Petition::~Petition()
{
}

void	Petition::setMethod(std::string method)
{
	this->method = method;
}

void	Petition::setPath(std::string path)
{
	this->path = path;
}

void	Petition::setProtocol(std::string protocol)
{
	this->protocol = protocol;
}

void	Petition::setType(std::string type)
{
	this->type = type;
}

void	Petition::setHeaders(std::string headers)
{
	this->headers = headers;
}

std::string Petition::getPath(void)
{
	return (this->path);
}

std::string Petition::getMethod(void)
{
	return (this->method);
}

std::string Petition::getType(void)
{
	return (this->type);
}

std::string Petition::getHeaders(void)
{
	return (this->headers);
}
