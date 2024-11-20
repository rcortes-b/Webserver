# include <cerrno>
# include <iostream>
# include <cstring>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <netdb.h>
# include <vector>
# include <csignal>
# include <sys/epoll.h>
# include <fcntl.h>
# include <map>
# include <vector>
# include <list>
# include "ServerConfig.hpp"

//placeholders
# define MAX_CONNECTIONS 1024
# define MAX_BUFFER_SIZE 1024

class ServerConfig;

typedef struct sockaddr_in	t_sockaddr_in;
typedef struct sockaddr t_sockaddr;
typedef struct addrinfo	t_addrinfo;
typedef std::vector<std::string>::iterator it_strvec;
typedef struct epoll_event t_epolle;


class SysError: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return (std::strerror(errno));
		}
};

class NoMsgError: public std::exception
{
public:
	public:
		virtual const char	*what() const throw()
		{
			return ("");
		}
};

class SimpleSocket
{
private:
	int serverSocket;
	t_addrinfo *addrinfo;
	ServerConfig *server;
	
public:
	SimpleSocket();
	SimpleSocket(std::string port);
	SimpleSocket(const SimpleSocket &other);
	SimpleSocket &operator=(const SimpleSocket &other);
	~SimpleSocket();

	void setSocket(std::string port, std::string host);
	void setServer(ServerConfig *server);
	ServerConfig *getServer(void) const;
	int acceptConnection(void);
	static int	readPetition(int clientFd, std::string &petition, ServerConfig &server);
	static int	readBody(std::string &petition, std::string token, int clientFd, ServerConfig &server, char *bodyContent, ssize_t bodySize);
	void clearData(void);
	int	getServerSocket(void) const;

};

void	connectServer( std::vector<ServerConfig> &server );

typedef std::map<int, SimpleSocket>::iterator it_socmap;