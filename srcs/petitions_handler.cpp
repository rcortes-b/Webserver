# include "../includes/Response.hpp"
# include "../includes/ServerConfig.hpp"

void handlePetition(std::string petition, int socketFd, ServerConfig &server)
{
	Response resp(server);

	try
	{		
		resp.setUp(petition);
	}
	catch(const std::exception &e) { }

	resp.sendResponseMsg(socketFd);
}