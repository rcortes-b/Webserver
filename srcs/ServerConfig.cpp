#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig(void)
{
	_host = "DefaultHost";
	_max_size = "DefaultLimitSize";
	_root = "DefaultRoot";
	_autoindex = NONE;
	_isDefPort = false;
	_isDefHost = false;
	_isDefServerName = false;
	_isDefErrorPage = false;
	_isDefMaxSize = false;
	_isDefRoot = false;
	_isDefIndex = false;
	_isDefAutoIndex = false;
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
	_isDefPort = obj._isDefPort;
	_isDefHost = obj._isDefHost;
	_isDefServerName = obj._isDefServerName;
	_isDefErrorPage = obj._isDefErrorPage;
	_isDefMaxSize = obj._isDefMaxSize;
	_isDefRoot = obj._isDefRoot;
	_isDefIndex = obj._isDefIndex;
	_isDefAutoIndex = obj._isDefAutoIndex;

	return (*this);
}

ServerConfig::~ServerConfig(void)
{
	//eliminar lo que haga falta
}

void	ServerConfig::setPort(std::string line)
{
	if (_isDefPort)
		throw ThrowError("Error: Port has been already defined");
	if(!get_multiple_values(line, _port))
		throw ThrowError("Error: Port is not valid");
	else if (!check_ports(_port))
		throw ThrowError("Port numbers are not valid");
	_isDefPort = true;
}

void	ServerConfig::setHost(std::string line)
{
	if (_isDefHost)
		throw ThrowError("Error: Host has been already defined");
	if (!get_single_value(line, _host))
		throw ThrowError("Error: Host is not valid");
	_isDefHost = true;
}

void	ServerConfig::setServerName(std::string line)
{
	if (_isDefServerName)
		throw ThrowError("Error: Servername has been already defined");
	if (!get_multiple_values(line, _server_name))
		throw ThrowError("Error: Servername is not valid");
	_isDefServerName = true;
}

void	ServerConfig::setErrorPage(std::string line)
{
	if (_isDefErrorPage)
		throw ThrowError("Error: Error page has been already defined");
	if (!get_multiple_values(line, _error_page))
		throw ThrowError("Error: Error page is not valid");
	_isDefErrorPage = true;
}

void	ServerConfig::setMaxSize(std::string line)
{
	if (_isDefMaxSize)
		throw ThrowError("Error: Maximum size has been already defined");
	if (!get_single_value(line, _max_size))
		throw ThrowError("Error: Maximum size defined is not valid");
	_isDefMaxSize = true;
}

void	ServerConfig::setRoot(std::string line)
{
	if (_isDefRoot)
		throw ThrowError("Error: Root has been already defined");
	if (!get_single_value(line, _root))
		throw ThrowError("Error: Root is not valid");
	_isDefRoot = true;
}

void	ServerConfig::setIndexFile(std::string line)
{
	if (_isDefIndex)
		throw ThrowError("Error: Index has been already defined");
	if (!get_multiple_values(line, _index_file))
		throw ThrowError("Error: Index is not valid");
	_isDefIndex = true;
}

void	ServerConfig::setAutoIndex(std::string line)
{
	if (_isDefAutoIndex)
		throw ThrowError("Error: Autoindex has been already defined");
	if (get_AutoIndex(line, _autoindex))
		throw ThrowError("Error: Autoindex is not valid");
	_isDefAutoIndex = true;
}

std::vector<ServerLocation>	&ServerConfig::getLocation(void)
{
	return _location;
}

std::vector<std::string>	&ServerConfig::getPort(void)
{
	return _port;
}