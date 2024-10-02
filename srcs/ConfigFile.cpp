#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigFile.hpp"

#define KEY_AMOUNT 9

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
	return key.length();
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
			return ; //esto no se si es error o que la verdad, default value prevalece??????
	}
	switch (counter) {
		case 1:
				server.setPort(&line[i]);
				break ;
		case 2:
				server.setHost(&line[i]);
				break ;
		case 3:
				server.setServerName(&line[i]);
				break ;
		case 4:
				server.setErrorPage(&line[i]);
				break ;
		case 5:
				server.setMaxSize(&line[i]);
				break ;
		case 6:
				server.setRoot(&line[i]);
				break ;
		case 7:
				server.setIndexFile(&line[i]);
				break ;
		case 8:
				server.setAutoIndex(&line[i]);
				break ;
	};
}

static void parse_lines(std::string const &line, ServerConfig &server)
{

	size_t	key_size = 0;
	size_t	counter = -1;
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]))
		{
			key_size = get_key_size(&line[i]);
			while (++counter < KEY_AMOUNT)
			{
				if (!std::strncmp(&line[i], keys[counter].c_str(), key_size))
					break ;
				else if (counter + 1 == KEY_AMOUNT)
				{
					std::cout << "ERROR AQUI !!!!! TENER EN CUENTA" << std::endl;
					return ;
				}
			}
			if (!counter)
				std::cout << "aqui ira el parseo de locations" << std::endl;
			else
				setConfigValue(&line[i + keys[counter].size()], server, counter);
		}
	}
}
static ServerConfig	iterate_server_data(std::string line, std::fstream &file)
{
	ServerConfig	server;

	parse_lines(line, server);

	while (std::getline(file, line))
		parse_lines(line, server);

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
					{
						std::cout << "error: entre server y bracket o post bracket (same line) hay algo random\n";
						return servers;
					}
					else if (line[i] == '{')
						servers.push_back(iterate_server_data(&line[i + 1], file)); //se manda el restante de la linea para ver si hay algo tipo server { host: 8080 ...
				}
			}
			else if (!std::isspace(line[i]))
			{
				std::cout << "algo no cuadra xd\n";
				return servers;
			}
		}
	}

	return servers;
}
