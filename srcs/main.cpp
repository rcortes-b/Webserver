//include headers here
#include "../includes/SimpleSocket.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerLocation.hpp"
#include "../includes/ConfigFile.hpp"

/*
ERROR MESSAGES
	- "The file has not a valid format. \".conf format is required.\""
*/

static void	check_servers_ports(std::vector<ServerConfig>	&servers)
{
	if (servers.size() == 1)
		return ;
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end() - 1; it++) {
		for (std::vector<std::string>::iterator it_port = (*it).getPort().begin(); it_port != (*it).getPort().end(); it_port++) {
			for (std::vector<ServerConfig>::iterator it2 = it + 1; it2 != servers.end(); it2++) {
				for (std::vector<std::string>::iterator it_port2 = (*it2).getPort().begin(); it_port2 != (*it2).getPort().end(); it_port2++) {
					if (*it_port == *it_port2)
						throw ThrowError("Error: Multiple definitions of the same port in different servers");
				}
			}
		}
	}
}

static std::vector<ServerConfig>	parse_file(std::string &file)
{
	size_t	len = file.size();
	if (len < 6) //Comprueba que tiene un mínimo de carácteres para que sea .conf
		throw ThrowError("The file has not a valid format. \".conf format is required.\"");
	std::string	format = ".conf";
	len -= 1;
	for (unsigned int i = 0; i < 5; i++) { //Se asegura que es un .conf comparando los últimos carácteres
		if (file[len] != format[4 - i])
			throw ThrowError("The file has not a valid format. \".conf format is required.\"");
		len--;
	}

	std::fstream	f;
	f.open(file.c_str());
	if (!f)
		throw ThrowError("Failed opening the file.");
	return parse_server_data(f);
}

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1] || !*argv[1])
	{
		std::cerr << "Invalid number of arguments or argument is invalid." << std::endl;
		return (1);
	}
	try {

		std::string file = argv[1];
		std::vector<ServerConfig>	servers;
		servers = parse_file(file);
		check_servers_ports(servers);
		connectServer(servers);

	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}