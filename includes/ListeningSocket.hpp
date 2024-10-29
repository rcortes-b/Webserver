# include <iostream>
# include <map>
# include "ServerConfig.hpp"

class ListeningSocket
{
private:
	std::string buffer;
	ServerConfig *server;

public:
	ListeningSocket();
	ListeningSocket &operator=(const ListeningSocket &other);
	ListeningSocket(const ListeningSocket &other);
	~ListeningSocket();

	void setServer(ServerConfig *server);
	ServerConfig *getServer(void) const;
	void setBuffer(std::string new_buffer);
	std::string &getBuffer(void);
};

typedef std::map<int, ListeningSocket>::iterator it_lstsocmap;
