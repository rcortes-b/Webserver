#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig(void)
{
	//inicializar lo que haga falta
}

ServerConfig::ServerConfig(ServerConfig const &obj)
{
	if (this != &obj)
		*this = obj;
}

ServerConfig	&ServerConfig::operator=(ServerConfig const &obj)
{
	
}

ServerConfig::~ServerConfig(void)
{
	//eliminar lo que haga falta
}
