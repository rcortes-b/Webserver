# include "../includes/Response.hpp"
# include "../includes/ServerConfig.hpp"

void	handlePetition(std::string header, char* bodyContent, ssize_t bodySize, int socketFd, ServerConfig &server)
{
	Response resp(server);

	try
	{		
		resp.setUp(header, bodyContent, bodySize);
	}
	catch(const std::exception &e) { }

	resp.sendResponseMsg(socketFd);
}