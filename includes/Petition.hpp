# include <iostream>
# include <string>
# include <vector>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <unistd.h>
# include <cstring>

class Petition
{
private:
	std::string method;
	std::string path;
	std::string protocol;
	std::string type;
	std::string headers;
public:
	Petition();
	Petition(const Petition &other);
	Petition &operator=(const Petition &other);
	~Petition();

	void setMethod(std::string method);
	void setPath(std::string path);
	void setProtocol(std::string protocol);
	void setType(std::string type);
	void setHeaders(std::string headers);
	std::string getPath(void);
	std::string getMethod(void);
	std::string getType(void);
};