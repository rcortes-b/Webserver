#include "../includes/ServerRequests.hpp"

ServerRequests::ServerRequests(void)
{
	//inicializar lo que haga falta
}

ServerRequests::ServerRequests(ServerRequests const &obj)
{
	if (this != &obj)
		*this = obj;
}

ServerRequests	&ServerRequests::operator=(ServerRequests const &obj)
{
	(void)obj;
	return (*this);
}

ServerRequests::~ServerRequests(void)
{
	//eliminar lo que haga falta
}