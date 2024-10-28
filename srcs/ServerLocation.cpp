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
	//delete if necesary
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
	std::cout << limit << " " << i << " " << line.substr(limit, i - limit) << " " << line.size() <<"\n";
	std::cout << "Address of _route: " << &_route << "\n";

	_route = line.substr(limit, i - limit);
	std::cout << "made it \n";
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
	/*This does not make sense !!!!!
	while (i < line.size() && std::isspace(line[i])) {
		i++;
		if (line[i] == '{') {
			limit++;
			i++;
		}
	}
	if (limit != 1 && i != line.size())
		throw ThrowError("");*/
}

void	ServerLocation::setMethods(std::string line)
{
	unsigned int	front_limit = 0;
	unsigned int	back_limit = 0;

	if (_isDefMethods)
		throw ThrowError("Error: Methods has been already defined");
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (line[i] == ',' || line[i] == ';')
		{
			if (is_valid_method(line.substr(front_limit, back_limit)))
				_methods.push_back(line.substr(front_limit, back_limit));
			else {
				custom_msg = "Error in location nº" + numToStr(location_amount) +": Method is not valid";
				throw ThrowError(static_cast<std::string const>(custom_msg));
			}
		}
		else if (i > 0 && std::isspace(line[i]) && std::isalnum(line[i - 1]))
			back_limit = i;
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
		if (line[i] == ',' || line[i] == ';')
		{
			if (is_valid_extension(line.substr(front_limit, back_limit)))
				_cgi_extension.push_back(line.substr(front_limit, back_limit));
			else {
			custom_msg = "Error in location nº" + numToStr(location_amount) +": CGI extension definition is not valid";
			throw ThrowError(static_cast<std::string const>(custom_msg));
		}
		}
		else if (i > 0 && std::isspace(line[i]) && std::isalnum(line[i - 1]))
			back_limit = i;
		else if ((i == 0 && std::isalnum(line[i])) || (i > 0 && std::isspace(line[i - 1]) && std::isalnum(line[i])))
			front_limit = i;
	}
	if (!_cgi_extension.size()) {
		custom_msg = "Error in location nº" + numToStr(location_amount) +": cgi_extension line is empty";
		throw ThrowError(static_cast<std::string const>(custom_msg));
	}
	_isDefCgi = true;
}

// tendre que checkear todas las lineas que no haya un closing bracket
	//primera linea se usara setRoute, ya que se supone que la primera tendra la ruta donde aplicar lo definido y un open bracket

static void	setLocation(std::string key, ServerLocation &location)
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
				location.setMethods(key);
				break;
			case 1:
				location.setCgi(key);
				break;
			case 2:
				location.setRedirect(key);
				break;
			case 3:
				location.setRoot(key);
				break;
			case 4:
				location.setIndex(key);
				break;
			case 5:
				location.setAutoIndex(key);
				break;
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
		}
		else
			setLocation(line.substr(key_pos, i - key_pos), location);
	}
	server.getLocation().push_back(location);
}