#ifndef SERVERLOCATION_HPP
# define SERVERLOCATION_HPP

# include "ConfigFile.hpp"
# include "ServerConfig.hpp"

class ServerConfig;

void	parse_location(std::fstream &file, std::string line, ServerConfig &server);

class ServerLocation {

	private:
			std::string					_route;
			std::vector<std::string>	_methods; //allow_methods GET, POST
			std::vector<std::string>	_cgi_extension; //cgi_extension py, php
			std::string					_redirect; //return https://42barcelona.com
			std::string					_root; //root /var/www/web1
			std::string					_index; //index index.html
			unsigned int				_autoindex; //autoindex ON

			//this booleans determines if a value has been already defined or not
			bool						_isDefMethods;
			bool						_isDefRedirect;
			bool						_isDefRoot;
			bool						_isDefAutoIndex;
			bool						_isDefIndex;
			bool						_isDefCgi;

	public:
			ServerLocation( void );
			ServerLocation( ServerLocation const &obj );
			ServerLocation &operator=( ServerLocation const &obj );
			~ServerLocation( void );

			void	setRoute( std::string line );
			void	setMethods( std::string line );
			void	setRedirect( std::string line );
			void	setRoot( std::string line );
			void	setAutoIndex( std::string line );
			void	setIndex( std::string line );
			void	setCgi( std::string line );

	/*class	LocationException : public std::exception {
		private:
				std::string _err_msg;
		public:
			LocationException( std::string const err_msg) {
				_err_msg = err_msg;
			}
			virtual const char	*what( void ) const throw() {
				return _err_msg.c_str();
			}
	};*/
};

#endif