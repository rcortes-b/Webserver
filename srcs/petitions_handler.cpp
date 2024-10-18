# include "../includes/Response.hpp"

void handlePetition(std::string petition, int socketFd)
{
	Response resp;

	try
	{		
		resp.setUp(petition);
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	resp.sendResponseMsg(socketFd);
}