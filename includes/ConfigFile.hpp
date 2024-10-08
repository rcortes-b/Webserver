#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <fstream>
# include "ServerConfig.hpp"

std::vector<ServerConfig>	parse_server_data(std::fstream &file);

#endif