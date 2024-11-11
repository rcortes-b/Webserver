# include "../includes/Petition.hpp"
# include "../includes/ServerConfig.hpp"

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
	ServerLocation location;
	std::string	protocol;
	std::string	statusCode;
	std::string	statusMsg;
	std::string	contentType;
	std::vector<std::string> heads;
	char *body;
	int	bodySize;
	

	void handleMethod(std::string method);
	void handlePath(std::string path);
	void handleProtocol(std::string protocol);
	void handleHeaders(std::string headers);
	void setLocation(void);

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
	void doGet(char *path);
};