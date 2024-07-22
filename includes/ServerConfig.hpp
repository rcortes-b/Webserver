#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <stdbool.h>
# include <cstring>

class ServerConfig {
	private:
			unsigned int	_port; //Esto puede ser un contenedor, depende de si hay que almacenar más de un tipo de puerto
			std::string		_host;
			std::string		_server_name;
			// (...)

	public:
			ServerConfig( void );
			ServerConfig( ServerConfig const &obj );
			ServerConfig &operator=( ServerConfig const &obj );
			~ServerConfig( void );
};

#endif