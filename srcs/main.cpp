//include headers here
#include "../includes/SimpleSocket.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerLocation.hpp"
#include "../includes/ConfigFile.hpp"

/*
ERROR MESSAGES
	- "The file has not a valid format. \".conf format is required.\""
*/

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
		//for (std::vector<ServerConfig>::iterator it = servers)
		connectServer(servers);

	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}