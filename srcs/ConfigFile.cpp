# include "../includes/ServerConfig.hpp"
# include "../includes/ServerLocation.hpp"
# include "../includes/ConfigFile.hpp"

#define KEY_AMOUNT 9

unsigned int	location_amount = 0;
std::string		custom_msg = "";

std::string	const keys[KEY_AMOUNT] = { "location",
						"port",
						"host",
						"server_name",
						"error_page",
						"client_max_body_size",
						"root",
						"index",
						"autoindex"
};

static size_t	get_key_size(std::string const &key)
{
	size_t	i = 0;
	while (i < key.size() && !std::isspace(key[i]))
		i++;
	return i;
}

/*
Las location son especiales, el port tambien y el autoindex tambien. El resto son todos iguales

Se me ocurre que en vez de hacer un setter convencional podría hacer un setter desarrollado para cada uno

Tengo que mirar que sea en la misma linea, si no esta el value de la key en la misma linea es error. Es decir, tengo que buscar el punto coma;
*/

static void	setConfigValue(std::string const &line, ServerConfig &server, unsigned int counter)
{
	unsigned int	i = 0;
	for (; i < line.size(); i++) // o no sea igual a ;
	{
		if (!std::isspace(line[i]))
			break ;
		else if (line[i] == ';')
			throw ThrowError("Error: Key has not definitions");
	}
	switch (counter) {
		case 1:
		//std::cout << "setPort in server" << std::endl;
				server.setPort(&line[i]);
				break ;
		case 2:
		//std::cout << "setHosy in server" << std::endl;
				server.setHost(&line[i]);
				break ;
		case 3:
		//std::cout << "setServerName in server" << std::endl;
				server.setServerName(&line[i]);
				break ;
		case 4:
		//std::cout << "setErrorPage in server" << std::endl;
				server.setErrorPage(&line[i]);
				break ;
		case 5:
		//std::cout << "setMaxSize in server" << std::endl;
				server.setMaxSize(&line[i]);
				break ;
		case 6:
		//std::cout << "setRoot in server" << std::endl;
				server.setRoot(&line[i]);
				break ;
		case 7:
		//std::cout << "setIndexFile in server" << std::endl;
				server.setIndexFile(&line[i]);
				break ;
		case 8:
		//std::cout << "setAutoIndex in server" << std::endl;
				server.setAutoIndex(&line[i]);
				break ;
	};
}

static bool parse_lines(std::string const &line, std::fstream &file, ServerConfig &server)
{
	size_t	key_size = 0;
	size_t	counter = -1;
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]) && line[i] != '}')
		{
			key_size = get_key_size(&line[i]);
			while (++counter < KEY_AMOUNT)
			{
				if (!std::strncmp(&line[i], keys[counter].c_str(), key_size))
					break ;
				else if (counter + 1 == KEY_AMOUNT) {
					custom_msg = "Error in server: Key " + line.substr(i, key_size) + " is not valid";
					throw ThrowError(static_cast<std::string const>(custom_msg));
				}
			}
			if (!counter)
				parse_location(file, &line[i + keys[counter].size()], server);
			else
				setConfigValue(&line[i + keys[counter].size()], server, counter);
			return true;
		}
		else if (line[i] == '}') {
			i++;
			while (i < line.size() && std::isspace(line[i]))
				i++;
			if (i != line.size())
				throw ThrowError("Error in server: Characters after \'}\' are not allowed");
			return false;
		}
	}
	return true;
}
static ServerConfig	iterate_server_data(std::string line, std::fstream &file)
{
	ServerConfig	server;

	while (std::getline(file, line)) {
		if (!parse_lines(line, file, server))
			return server;
	}
	return server;
}

std::vector<ServerConfig>	parse_server_data(std::fstream &file)
{
	std::vector<ServerConfig>	servers;
	std::string	line = "";

	while (std::getline(file, line))
	{
		for (unsigned int i = 0; i < line.size(); i++)
		{
			if (!std::strncmp(&line[i], "server", 6))
			{
				i += 6;
				for (;i < line.size(); i++)
				{
					if (!std::isspace(line[i]) && line[i] != '{')
						throw ThrowError("Error in server: Characters between \'server\' and \'{\' are not allowed");
					else if (line[i] == '{') {
						i++;
						while (i < line.size() && std::isspace(line[i]))
							i++;
						if (i != line.size())
							throw ThrowError("Error in server: Characters after \'{\' are not allowed");
						location_amount = 0;
						servers.push_back(iterate_server_data(line, file));
						//this is to delete
						std::cout << "Servers Amount: " << servers.size() << std::endl;
						for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
						{
							std::cout << "----- SERVER -----" << std::endl;
							std::cout << "Location Size: " << (*it).getLocation().size() << std::endl;
							for (std::vector<ServerLocation>::iterator itLoc = (*it).getLocation().begin(); itLoc != (*it).getLocation().end(); itLoc++)
							{
								std::cout << "----- LOCATION -----" << std::endl;
								std::cout << "Route: " << (*itLoc).getRoute() << std::endl;
								std::cout << "Redirect: " << (*itLoc).getRedirect() << std::endl;
								std::cout << "Root: " << (*itLoc).getRoot() << std::endl;
								std::cout << "Index: " << (*itLoc).getIndex() << std::endl;
								std::cout << "AutoIndex: " << (*itLoc).getAutoIndex() << std::endl;
								for (std::vector<std::string>::iterator itMet = (*itLoc).getMethods().begin(); itMet != (*itLoc).getMethods().end(); itMet++)
									std::cout << "Method: " << *itMet << std::endl;
								std::cout << "CGI size: " << (*itLoc).getCgiExtension().size() << std::endl;
								for (std::vector<std::string>::iterator itCgi = (*itLoc).getCgiExtension().begin(); itCgi != (*itLoc).getCgiExtension().end(); itCgi++)
									std::cout << "Cgi: " << *itCgi << std::endl;
								std::cout << "----- END LOCATION -----" << std::endl;
							}
							for (std::vector<std::string>::iterator itPort = (*it).getPort().begin(); itPort != (*it).getPort().end(); itPort++)
								std::cout << "Port of server: " << *itPort << std::endl;
							for (std::vector<std::string>::iterator itSn = (*it).getServerName().begin(); itSn != (*it).getServerName().end(); itSn++)
								std::cout << "ServerName of server: " << *itSn << std::endl;
							for (std::vector<std::string>::iterator itEp = (*it).getErrorPage().begin(); itEp != (*it).getErrorPage().end(); itEp++)
								std::cout << "Error Page of server: " << *itEp << std::endl;
							std::cout << "Index: " << (*it).getIndex() << std::endl;
							std::cout << "Host: " << (*it).getHost() << std::endl;
							std::cout << "Max Size: " << (*it).getMaxSize() << std::endl;
							std::cout << "Root: " << (*it).getRoot() << std::endl;
							std::cout << "AutoIndex: " << (*it).getAutoIndex() << std::endl;
							std::cout << "----- END OF SERVER -----" << std::endl;
						}
					}
				}
			}
			else if (!std::isspace(line[i]))
				throw ThrowError("Error in file: Unknown file content");
		}
	}
	
	return servers;
}


/* Auxilar Funcitons to class getters of the Config File */

bool	is_valid_method(std::string const &method)
{
	std::string	valid_method[3] = { "GET",
									"POST",
									"DELETE"
											};
	for (unsigned int it = 0; it < 3; it++) {
		if (valid_method[it] == method)
			return true;
	}
	return false;
}

bool	is_valid_extension(std::string const &extension)
{
	std::string	valid_extension[8] = { "html",
									"css",
									"jpg",
									"js",
									"py",
									"php",
									"sh",
									"c"
											};
	for (unsigned int it = 0; it < 8; it++) {
		if (valid_extension[it] == extension)
			return true;
	}
	return false;
}


bool	get_single_value(std::string &line, std::string &value)
{
	unsigned int	front = 0;
	unsigned int	i = 0;

	while (i < line.size() && std::isspace(line[i]))
		i++;
	front = i;
	while (i < line.size() && !std::isspace(line[i]) && line[i] != ',' && line[i] != ';')
		i++;
	value = line.substr(front, i - front);
	if (!is_the_end(&line[i]))
		return false;
	return true;
}

/*
Esta funcion se usa para las variables que solo tienen que ser de un item, es decir, sin vectores donde pueden haber mas de uno.
Pues se usa para saber si todo lo que queda es espacios y un semicolon o hay mas bullshit
Cuando devuelve 'false' habra que tirar un throw
*/

bool	is_the_end(std::string line)
{
	unsigned int c = 0;

	for (unsigned int i = 0; i < line.size(); i++) {
		if (!std::isspace(line[i]) && line[i] != ';')
			return false;
		else if (line[i] == ';')
			c++;
	}
	if (!c || c > 1)
		return false;
	return true;
}

void	update_backlimit(std::string &line, unsigned int pos, unsigned int &back_limit)
{
	if (pos > 0 && ((std::isspace(line[pos]) && std::isalnum(line[pos - 1]))
		|| (line[pos] == ',' && std::isalnum(line[pos - 1]))
		|| (line[pos] == ';' && std::isalnum(line[pos - 1]))))
		back_limit = pos;
}

bool	get_AutoIndex(std::string &line, unsigned int &autoindex)
{
	unsigned int	i = 0;
	unsigned int	front = 0;
	while (std::isspace(line[i]))
		i++;
	front = i;
	while (i < line.size() && !std::isspace(line[i]) && line[i] != ',' && line[i] != ';')
		i++;
	if (line.substr(front, i - front) == "ON")
		autoindex = ON;
	else if (line.substr(0, i) == "OFF")
		autoindex = OFF;
	else {
		std::cout << line << " " <<  i <<line.substr(0, 1) << std::endl;
		return false;
	}
	if (!is_the_end(&line[i]))
		return false;
	return true;
}

bool	get_multiple_values(std::string &line, std::vector<std::string> &vector)
{
	unsigned int	front_limit = 0;
	unsigned int	back_limit = 0;
	std::cout << "Line: " << line << std::endl;
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (i > 0 && std::isspace(line[i - 1]) && (std::isalnum(line[i]) || line[i] == '.' || line[i] == '-' || line[i] == '~') && line[i] != ',' && line[i] != ';')
			front_limit = i;
		while (i < line.size() && !std::isspace(line[i]) && line[i] != ';' && line[i] != ',')
			i++;
		back_limit = i;
		if (line[i] == ',' || line[i] == ';')
			vector.push_back(line.substr(front_limit, back_limit - front_limit));
	}
	if (!vector.size())
		return false;
	return true;
}

bool	check_ports(std::vector<std::string> &vector)
{
	std::vector<std::string>::iterator	it = vector.begin();
	while (it != vector.end())
	{
		for (std::vector<std::string>::iterator	it2 = it + 1; it2 != vector.end(); it2++) {
			if (it2 == vector.end())
				break ;
			if (*it2 == *it)
				throw ThrowError("Error: Multiple definitions of the same port");
		}
		for (unsigned int i = 0; i < it->size(); i++)
		{
			if (!std::isdigit((*it)[i]))
				return false;
		}
		it++;
	}
	return true;
}

std::string	numToStr(unsigned int num)
{
	std::ostringstream	oss;

	oss << num;
	return oss.str();
}

unsigned long	strToulNum(std::string str)
{
	unsigned long	num = 0;

	for (unsigned int i = 0; i < str.size(); i++) {

		if (std::isdigit(str[i]))
			num = num * 10 + (str[i] - '0');
	}
	return num;
}

int	strToNum(std::string str)
{
	int	num = 0;

	for (unsigned int i = 0; i < str.size(); i++) {
		num = num * 10 + (str[i] - '0');
	}
	return num;
}
