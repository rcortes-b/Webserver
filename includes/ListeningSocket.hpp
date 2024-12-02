# include <iostream>
# include <map>
# include <sys/socket.h>
# include "ServerConfig.hpp"
# include "Response.hpp"

# define MAX_BUFFER_SIZE 1024

class ListeningSocket
{
private:
	std::string bufferStr;
	char		*bufferChar;
	ssize_t		bufferCharSize;
	ServerConfig *server;

	char		*firstBody;
	ssize_t		firstBodySize;
	std::string	token;

public:
	ListeningSocket();
	ListeningSocket &operator=(const ListeningSocket &other);
	ListeningSocket(const ListeningSocket &other);
	~ListeningSocket();


	int	readPetition(int clientFd);
	int	readHeader(int clientFd);
	int	readBody(int clientFd);

	void setServer(ServerConfig *server);
	ServerConfig *getServer(void) const;
	void setBufferStr(std::string new_bufferStr);
	std::string &getBufferStr(void);
	char *getBufferChar(void);
	ssize_t &getBufferCharSize(void);
};

typedef std::map<int, ListeningSocket>::iterator it_lstsocmap;
