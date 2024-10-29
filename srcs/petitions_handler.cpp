# include "../includes/Response.hpp"
# include "../includes/ServerConfig.hpp"

void handlePetition(std::string petition, int socketFd, ServerConfig &server)
{
	Response resp(server);

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