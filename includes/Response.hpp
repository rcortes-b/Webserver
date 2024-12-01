#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/Petition.hpp"
# include "../includes/ServerConfig.hpp"

# include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

void	handlePetition(std::string header, char* bodyContent, ssize_t bodySize, int socketFd, ServerConfig &server);

class BadPetition: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("Bad Petition");
		}
};

class Redirect: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("Redirectng...");
		}
};

class AutoIndex: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("Showing AutoIndex...");
		}
};

class CGI: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("Executing CGI...");
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
	void checkMethods(void);
	void handlePath(std::string path);
	void handleIndexes(std::string &path);
	void handleProtocol(std::string protocol);
	void handleHeaders(std::string headers);
	void setLocation(void);

public:
	Response();
	Response(ServerConfig &server);
	Response(Response &other);
	Response &operator=(Response &other);
	~Response();


	void setUp(std::string header, char *bodyContent, ssize_t bodySize);
	void setBadThrow(std::string statusCode, std::string statusMsg);
	void setRedirectThrow(std::string host);
	std::string setResponseHead(std::string &resp);
	void setFileName(std::string &newPath);
	void sendResponseMsg(int socketFd);
	void doGet(char *path);
	void doPost(std::ofstream &pathFile);
	void doAutoIndex(char *path);
	ServerLocation getLocation(void);
};

bool		is_cgi(Response &resp, char *path);
std::string	doCgi (char *path);

#endif