# include "../includes/Petition.hpp"
# include "../includes/ServerConfig.hpp"

// TEMPOAL VARIABLES
# define MAX_BODYSIZE 2000
# define MAX_HEADERSIZE 500

void	handlePetition(std::string petition, int socketFd, ServerConfig &server);

class BadPetition: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("Bad Petition");
		}
};

class Response
{
private:
	Petition petition;
	ServerConfig server;
	std::string	protocol;
	std::string	statusCode;
	std::string	statusMsg;
	std::string	contentType;
	std::vector<std::string> heads;
	

	void handleMethod(std::string method);
	void handlePath(std::string path);
	void handleProtocol(std::string protocol);
	void handleHeaders(std::string headers);

public:
	Response();
	Response(ServerConfig &server);
	Response(Response &other);
	Response &operator=(Response &other);
	~Response();


	void setUp(std::string petition);
	void setBadThrow(std::string statusCode, std::string statusMsg);
	std::string setResponseHead(std::string &resp);
	void sendResponseMsg(int socketFd);
	void doGet(char *path, struct stat &stat_buf, char *buffer_body);
};