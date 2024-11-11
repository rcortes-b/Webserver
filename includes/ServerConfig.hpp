#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "ServerLocation.hpp"
# include <iostream>
# include <stdbool.h>
# include <cstring>
# include <vector>
# include <fstream>

class ServerLocation;

enum	e_autoindex {
	NONE,
	ON,
	OFF
};

class ServerConfig {
	private:
			std::vector<ServerLocation>	_location;
			std::vector<std::string>	_port;
			std::string					_host;
			std::vector<std::string>	_server_name;
			std::vector<std::string>	_error_page;
			std::string					_max_size;
			std::string					_root;
			std::vector<std::string>	_index_file;
			unsigned int				_autoindex;
			// (...)

			//this booleans determines if a value has been already defined or not
			bool						_isDefPort;
			bool						_isDefHost;
			bool						_isDefServerName;
			bool						_isDefErrorPage;
			bool						_isDefMaxSize;
			bool						_isDefRoot;
			bool						_isDefIndex;
			bool						_isDefAutoIndex;

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

			std::vector<ServerLocation>	&getLocation( void );
			std::vector<std::string>	&getPort( void );
			std::string					&getHost( void );
			std::vector<std::string>	&getServerName( void );
			std::vector<std::string>	&getErrorPage( void );
			std::string					&getMaxSize( void );
			std::string					&getRoot( void );
			std::vector<std::string>	&getIndex( void );
			unsigned int				&getAutoIndex( void );

};

typedef std::vector<ServerConfig>::iterator it_servec;

#endif