# include "../includes/Petition.hpp"

// TEMPOAL VARIABLES
# define MAX_BODYSIZE 500
# define MAX_HEADERSIZE 500
# define ROOT "/home/mvallhon/Documents/C_Cursus/test_webserv_files"
void	handlePetition(std::string petition, int socketFd);

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
	std::string protocol;
	std::string	statusCode;
	std::string statusMsg;
	std::string contentType;
	std::vector<std::string> heads;

	void handleMethod(std::string method);
	void handlePath(std::string path);
	void handleProtocol(std::string protocol);
	void handleHeaders(std::string headers);

public:
	Response();
	Response(Response &other);
	Response &operator=(Response &other);
	~Response();


	void setUp(std::string petition);
	void setBadThrow(std::string statusCode, std::string statusMsg);
	std::string setResponseHead(std::string &resp);
	void sendResponseMsg(int socketFd);
	void doGet(char *path, struct stat &stat_buf, char *buffer_body);
};