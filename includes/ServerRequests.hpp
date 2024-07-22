#ifndef SERVERREQUESTS_HPP
# define SERVERREQUESTS_HPP

# include <iostream>
# include <stdbool.h>
# include <cstring>

class ServerRequests {
	private:
			std::string	_method;
			std::string	_url;
			std::string	_version;
			// ( ... ) --> ver que recibe como peticion, hacer un getnextline y ver el contenido linea x linea
	public:
			ServerRequests( void );
			ServerRequests( ServerRequests const &obj );
			ServerRequests &operator=( ServerRequests const &obj );
			~ServerRequests( void );
};

#endif