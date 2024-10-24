#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig(void)
{
	_host = "DefaultHost";
	_max_size = "DefaultLimitSize";
	_root = "DefaultRoot";
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
	if(!get_multiple_values(line, _port))
		throw ThrowError("Error: Port is not valid");
	else if (!check_ports(_port))
		throw ThrowError("Port numbers are not valid");
}

void	ServerConfig::setHost(std::string line)
{
	if (!get_single_value(line, _host))
		throw ThrowError("Error: Host is not valid");
}

void	ServerConfig::setServerName(std::string line)
{
	if (!get_multiple_values(line, _server_name))
		throw ThrowError("Error: Servername is not valid");
}

void	ServerConfig::setErrorPage(std::string line)
{
	if (!get_multiple_values(line, _error_page))
		throw ThrowError("Error: Error page is not valid");
}

void	ServerConfig::setMaxSize(std::string line)
{
	if (!get_single_value(line, _max_size))
		throw ThrowError("Error: Maximum size defined is not valid");
}

void	ServerConfig::setRoot(std::string line)
{
	if (!get_single_value(line, _root))
		throw ThrowError("Error: Root is not valid");
}

void	ServerConfig::setIndexFile(std::string line)
{
	if (!get_multiple_values(line, _index_file))
		throw ThrowError("Error: Index is not valid");
}

void	ServerConfig::setAutoIndex(std::string line)
{
	if (get_AutoIndex(line, _autoindex))
		throw ThrowError("Error: Autoindex is not valid");
}

std::vector<ServerLocation>	&ServerConfig::getLocation(void)
{
	return _location;
}

std::vector<std::string>	&ServerConfig::getPort(void)
{
	return _port;
}