#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <stdbool.h>
# include <cstring>

class ServerConfig {
	private:
			unsigned int	port; //Esto puede ser un contenedor, depende de si hay que almacenar más de un tipo de puerto
			std::string		host;
			std::string		server_name;
			// (...)

	public:
};

#endif