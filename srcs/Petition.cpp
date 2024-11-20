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
	this->bodyContent = other.bodyContent;
	this->bodySize = other.bodySize;
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
		this->bodyContent = other.bodyContent;
		this->bodySize = other.bodySize;
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

void	Petition::setBodyContent(char *bodyContent)
{
	this->bodyContent = bodyContent;
}

void	Petition::setBodySize(ssize_t bodySize)
{
	this->bodySize = bodySize;
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

char *Petition::getBodyContent(void)
{
	return (this->bodyContent);
}

ssize_t	Petition::getBodySize(void)
{
	return (this->bodySize);
}
