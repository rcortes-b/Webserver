#include "../includes/ServerLocation.hpp"

ServerLocation::ServerLocation(void)
{
	_route = "";
	_redirect = "";
	_root = "";
	_autoindex = NONE;
	_index = "";
	_isDefMethods = false;
	_isDefRedirect = false;
	_isDefRoot = false;
	_isDefAutoIndex = false;
	_isDefIndex = false;
	_isDefCgi = false;


}

ServerLocation::ServerLocation(ServerLocation const &obj)
{
	if (this != &obj)
		*this = obj;
}

ServerLocation	&ServerLocation::operator=(ServerLocation const &obj)
{
	_route = obj._route;
	_methods = obj._methods;
	_cgi_extension = obj._cgi_extension;
	_redirect = obj._redirect;
	_root = obj._root;
	_autoindex = obj._autoindex;
	_index = obj._index;
	_isDefMethods = obj._isDefMethods;
	_isDefRedirect = obj._isDefRedirect;
	_isDefRoot = obj._isDefRoot;
	_isDefAutoIndex = obj._isDefAutoIndex;
	_isDefIndex = obj._isDefIndex;
	_isDefCgi = obj._isDefCgi;

	return *this;
}

ServerLocation::~ServerLocation(void)
{
}

/* comma as separator and semi colon as finisher */

void	ServerLocation::setRoute(std::string line)
{
	unsigned int limit = 0;
	unsigned int i = 0;
	while (i < line.size() && std::isspace(line[i]))
		i++;
	limit = i;
	while (i < line.size() && !std::isspace(line[i]) && line[i] != '{')
		i++;
	if (i == line.size() || i == limit) {
		custom_msg = "Error in location n" + numToStr(location_amount) +": Location route is wrong defined";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_route = line.substr(limit, i - limit);
	if (line[i] == '{') {
		i += 1;
		while (i < line.size() && std::isspace(line[i]))
			i++;
		if (i != line.size()) {
			custom_msg = "Error in location n" + numToStr(location_amount) +": Characters after \'{\' are not allowed";
			throw ThrowError(static_cast<std::string const>(custom_msg));
		}
	}
	limit = 0;
}

void	ServerLocation::setMethods(std::string line)
{
	unsigned int	front_limit = 0;
	unsigned int	back_limit = 0;

	if (_isDefMethods)
		throw ThrowError("Error: Methods has been already defined");
	for (unsigned int i = 0; i < line.size(); i++)
	{
		update_backlimit(line, i, back_limit);
		if (line[i] == ',' || line[i] == ';')
		{
			if (is_valid_method(line.substr(front_limit, back_limit - front_limit)))
				_methods.push_back(line.substr(front_limit, back_limit - front_limit));
			else {
				custom_msg = "Error in location nº" + numToStr(location_amount) +": Method is not valid";
				throw ThrowError(static_cast<std::string const>(custom_msg));
			}
		}
		else if ((i == 0 && std::isalnum(line[i])) || (i > 0 && std::isspace(line[i - 1]) && std::isalnum(line[i])))
			front_limit = i;
	}
	if (!_methods.size()) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": Allow_methods line is empty";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefMethods = true;
}

void	ServerLocation::setRedirect(std::string line)
{
	if (_isDefRedirect)
		throw ThrowError("Error: Return has been already defined");
	if (!get_single_value(line, _redirect)) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": Return definition is not valid";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefRedirect = true;
}


void	ServerLocation::setRoot(std::string line)
{
	if (_isDefRoot)
		throw ThrowError("Error: Root has been already defined");
	if (!get_single_value(line, _root)) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": Root definition is not valid";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefRoot = true;
}

void	ServerLocation::setRootSimple(std::string line)
{
	if (_isDefRoot)
		throw ThrowError("Error: Root has been already defined");
	this->_root = line;
	_isDefRoot = true;
}

void	ServerLocation::setIndexSimple(std::string line)
{
	if (_isDefIndex)
		throw ThrowError("Error: Index has been already defined");
	this->_index = line;
	_isDefIndex = true;
}

void	ServerLocation::setAutoIndex(std::string line)
{
	if (_isDefAutoIndex)
		throw ThrowError("Error: Autoindex has been already defined");
	if (!get_AutoIndex(line, _autoindex)) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": Autoindex definition is not valid";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefAutoIndex = true;
}

void	ServerLocation::setAutoIndex(unsigned int num)
{
	if (_isDefAutoIndex)
		throw ThrowError("Error: Autoindex has been already defined");
	this->_autoindex = num;
	_isDefAutoIndex = true;
}

void	ServerLocation::setIndex(std::string line)
{
	if (_isDefIndex)
		throw ThrowError("Error: Index has been already defined");
	if (!get_single_value(line, _index)) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": Index definition is not valid";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefIndex = true;
}

void	ServerLocation::setCgi(std::string line)
{
	unsigned int	front_limit = 0;
	unsigned int	back_limit = 0;

	if (_isDefCgi)
		throw ThrowError("Error: CGI has been already defined");
	for (unsigned int i = 0; i < line.size(); i++)
	{
		update_backlimit(line, i, back_limit);
		if (line[i] == ',' || line[i] == ';')
		{
			if (is_valid_extension(line.substr(front_limit, back_limit - front_limit)))
				_cgi_extension.push_back(line.substr(front_limit, back_limit - front_limit));
			else {
				custom_msg = "Error in location nº" + numToStr(location_amount) +": CGI extension definition is not valid";
				throw ThrowError(static_cast<std::string const>(custom_msg));
			}
		}
		else if ((i == 0 && std::isalnum(line[i])) || (i > 0 && std::isspace(line[i - 1]) && std::isalnum(line[i])))
			front_limit = i;
	}
	if (!_cgi_extension.size()) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": cgi_extension line is empty";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefCgi = true;
}

static void	setLocation(std::string key, ServerLocation &location, std::string line)
{
	std::string key_options[6] = {
		"allow_methods",
		"cgi_extension",
		"return",
		"root",
		"index",
		"autoindex"
	};
	unsigned int i = 0;
	for (; i < 6; i++) {
		if (key_options[i] == key) {
			switch (i)
			{
			case 0:
				location.setMethods(line);
				return ;
			case 1:
				location.setCgi(line);
				return ;
			case 2:
				location.setRedirect(line);
				return ;
			case 3:
				location.setRoot(line);
				return ;
			case 4:
				location.setIndex(line);
				return ;
			case 5:
				location.setAutoIndex(line);
				return ;
			}
		}
	}
	if (i == 6) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": " + key + " is not valid";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
}

void	parse_location(std::fstream &file, std::string line, ServerConfig &server)
{
	ServerLocation	location;
	unsigned int	i = 0;
	unsigned int	key_pos = 0;
	
	location.setRoute(line);
	while (std::getline(file, line)) {
		for (i = 0; i < line.size() && std::isspace(line[i]); i++);
		key_pos = i;
		for (; i < line.size() && !std::isspace(line[i]) && line[i] != '}'; i++);
		if (line[i] == '}') {
			i += 1;
			for (; i < line.size(); i++) {
				if (!std::isspace(line[i])) {
					custom_msg = "Error in location nº" + numToStr(location_amount) +": Characters after \'}\' are not allowed";
					throw ThrowError(static_cast<std::string const>(custom_msg));
				}
			}
			location_amount++;
			server.getLocation().push_back(location);
			break ;
		}
		else
			setLocation(line.substr(key_pos, i - key_pos), location, &line[i]);
	}
}

std::string	&ServerLocation::getRoute(void)
{
	return _route;
}

std::vector<std::string>	&ServerLocation::getMethods(void)
{
	return _methods;
}

std::vector<std::string>	&ServerLocation::getCgiExtension(void)
{
	return _cgi_extension;
}

std::string	&ServerLocation::getRedirect(void)
{
	return _redirect;
}

std::string	&ServerLocation::getRoot(void)
{
	return _root;
}

std::string	&ServerLocation::getIndex(void)
{
	return _index;
}

unsigned int	&ServerLocation::getAutoIndex(void)
{
	return _autoindex;
}