#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <stdbool.h>
# include <cstring>
# include <vector>

enum	e_autoindex {
	NONE,
	ON,
	OFF
};

class serverLocation {
	private:
	public:
};

class ServerConfig {
	private:
			std::vector<serverLocation>	_location;
			std::vector<std::string>	_port;
			std::string					_host;
			std::string					_server_name;
			std::string					_error_page;
			std::string					_max_size;
			std::string					_root;
			std::string					_index_file;
			unsigned int				_autoindex;
			// (...)

	public:
			ServerConfig( void );
			ServerConfig( ServerConfig const &obj );
			ServerConfig &operator=( ServerConfig const &obj );
			~ServerConfig( void );

			//void	setLocation();
			void	setPort( std::string line);
			void	setHost( std::string line);
			void	setServerName( std::string line);
			void	setErrorPage( std::string line);
			void	setMaxSize( std::string line);
			void	setRoot( std::string line);
			void	setIndexFile( std::string line);
			void	setAutoIndex( std::string line);

};

#endif