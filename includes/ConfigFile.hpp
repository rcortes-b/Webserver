#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include "ServerConfig.hpp"
# include "ServerLocation.hpp"

extern unsigned int		location_amount;
extern std::string		custom_msg;

class ServerConfig;

std::vector<ServerConfig>	parse_server_data(std::fstream &file);

bool	is_valid_method( std::string  const &method );
bool	is_valid_extension(std::string  const &extension);
bool	is_the_end( std::string line );
bool	get_single_value( std::string &line, std::string &value );
bool	get_multiple_values(std::string &line, std::vector<std::string> &vector);
bool	get_AutoIndex(std::string &line, unsigned int &autoindex);
bool	check_ports(std::vector<std::string> &vector);

void	update_backlimit(std::string &line, unsigned int pos, unsigned int &back_limit);

std::string		numToStr(unsigned int num);
unsigned long	strToulNum(std::string str);
int				strToNum(std::string str);

class ThrowError : public std::exception {
	private:
			std::string	_err_msg;
	public:
			ThrowError( std::string const err_msg ) {
				_err_msg = err_msg;
			}
			virtual ~ThrowError( void ) throw() {};
			virtual const char	*what( void ) const throw() {
				return _err_msg.c_str();
			}
};

#endif