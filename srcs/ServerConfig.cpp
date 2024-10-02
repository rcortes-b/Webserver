#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig(void)
{
	_host = "DefaultHost";
	_server_name = "DefaultServerName";
	_error_page = "DefaultErrorPage";
	_max_size = "DefaultLimitSize";
	_root = "DefaultRoot";
	_index_file = "DefaultIndexFile";
	_autoindex = NONE;
}

ServerConfig::ServerConfig(ServerConfig const &obj)
{
	if (this != &obj)
		*this = obj;
}

ServerConfig	&ServerConfig::operator=(ServerConfig const &obj)
{
	_location = obj._location;
	_port = obj._port;
	_host = obj._host;
	_server_name = obj._server_name;
	_error_page = obj._error_page;
	_max_size = obj._max_size;
	_root = obj._root;
	_index_file = obj._index_file;
	_autoindex = obj._autoindex;
	return (*this);
}

ServerConfig::~ServerConfig(void)
{
	//eliminar lo que haga falta
}


void	ServerConfig::setPort(std::string line)
{

}

void	ServerConfig::setHost(std::string line)
{

}

void	ServerConfig::setServerName(std::string line)
{

}

void	ServerConfig::setErrorPage(std::string line)
{

}

void	ServerConfig::setMaxSize(std::string line)
{

}

void	ServerConfig::setRoot(std::string line)
{

}

void	ServerConfig::setIndexFile(std::string line)
{

}

void	ServerConfig::setAutoIndex(std::string line)
{

}
