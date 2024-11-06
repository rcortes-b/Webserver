# include "../includes/Response.hpp"
# include "../includes/ConfigFile.hpp"

Response::Response()
{
	this->statusCode = "200";
	this->statusMsg = "OK";
	this->contentType = "text/plain";
	this->body = "";
	this->heads.push_back("Server: webserv");
}

Response::Response(ServerConfig &server)
{
	this->statusCode = "200";
	this->statusMsg = "OK";
	this->contentType = "text/plain";
	this->server = server;
	this->body = "";
	this->heads.push_back("Server: webserv");
}

Response::Response(Response &other)
{
	this->petition = other.petition;
	this->protocol = other.protocol;
	this->statusCode = other.statusCode;
	this->statusMsg = other.statusMsg;
	this->contentType = other.contentType;
	for (std::vector<std::string>::iterator it = other.heads.begin(); it != other.heads.end(); it++)
		this->heads.push_back(*it);
}

Response &Response::operator=(Response &other)
{
	if (this != &other)
	{
		this->petition = other.petition;
		this->protocol = other.protocol;
		this->statusCode = other.statusCode;
		this->statusMsg = other.statusMsg;
		this->contentType = other.contentType;
		this->heads.clear();
		for (std::vector<std::string>::iterator it = other.heads.begin(); it != other.heads.end(); it++)
			this->heads.push_back(*it);
	}

	return (*this);
}

Response::~Response()
{
}

void	Response::setBadThrow(std::string statusCode, std::string statusMsg)
{
	std::cout << statusMsg << '\n';
	this->protocol = "HTTP/1.1";
	this->statusCode = statusCode;
	this->statusMsg = statusMsg;
	this->contentType = "text/html";
	
	std::string path = "./www/errors/" + this->statusCode + ".html";
	std::vector<std::string> errorPage = this->server.getErrorPage();
	int errorPageSize = errorPage.size();
	for (int i = 0; i < errorPageSize - 1; i++)
	{
		if (this->statusCode == errorPage[i] && access(errorPage[errorPageSize - 1].c_str(), R_OK) == 0)
		{
			path = errorPage[errorPageSize - 1];
			break;
		}
	}
	this->doGet(const_cast<char *>(path.c_str()));
	throw BadPetition();
}

void	Response::setUp(std::string petition)
{
	int token;
	
	if (((const unsigned long)(token = petition.find("\r\n"))) == std::string::npos)
		this->setBadThrow("400", "Bad Request");
	std::string statusLine = petition.substr(0, token + 2);
	
	int from;
	for (size_t i = 0; i < 3; i++)
	{
		from = 0;
		if (i == 0)
		{
			if (((const unsigned long)(token = statusLine.find(" "))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");

			std::string method = statusLine.substr(from, token);
			this->handleMethod(method);
			statusLine = statusLine.substr(++token);
		}
		else if (i == 1)
		{
			if (((const unsigned long)(token = statusLine.find(" "))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");

			std::string path = statusLine.substr(from, token);
			this->handlePath(path);
			statusLine = statusLine.substr(++token);
		}
		else if (i == 2)
		{
			if (((const unsigned long)(token = statusLine.find("\r\n"))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");

			std::string protocol = statusLine.substr(from, token);
			this->handleProtocol(protocol);
			token += 2;
			statusLine = statusLine.substr(token);
		}
	}
	if (((const unsigned long)(from = statusLine.find("\0"))) == std::string::npos)
			this->setBadThrow("400", "Bad Request");
	
	std::string headers = petition.substr(from);
	this->handleHeaders(headers);
}

void	Response::handleMethod(std::string method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		this->setBadThrow("405", "Method Not Allowed");
	this->petition.setMethod(method);
}

void Response::handlePath(std::string path)
{
	int len = path.length();

	if (len > 0 && path[len - 1] != '/' && path.find('.') == std::string::npos)
		path.append("/");
	len = path.length();

	if(len > 0 && (path[len - 1] == '/'))
	{
		this->petition.setPath(path.append("index.html"));
		this->contentType = "text/html";
	}
	else if (len > 4 && path[len - 5] == '.' && path[len - 4] == 'h' && path[len - 3] == 't' && path[len - 2] == 'm' && path[len - 1] == 'l')
	{
		this->petition.setPath(path);
		this->contentType = "text/html";
	}
	else if (len > 3 && ((path[len - 4] == '.' && path[len - 3] == 'j' && path[len - 2] == 'p' && path[len - 1] == 'g') || 
	(path[len - 3] == 'J' && path[len - 2] == 'P' && path[len - 1] == 'G')))
	{
		this->petition.setPath(path);
		this->contentType = "image/jpeg";			
	}
	else if (len > 2 && path[len - 3] == '.' && path[len - 2] == 'j' && path[len - 1] == 's')
	{
		this->petition.setPath(path);
		this->contentType = "text/javascript";
	}
	else if (len > 3 && path[len - 4] == '.' && path[len - 3] == 'c' && path[len - 2] == 's' && path[len - 1] == 's')
	{
		this->petition.setPath(path);
		this->contentType = "text/css";
	}
	else
		this->setBadThrow("415", "Unsuppported Media Type");
}

void Response::handleProtocol(std::string protocol)
{
	if (protocol != "HTTP/1.1")
	{
		this->heads.push_back("Upgrade: HTTP/1.1");
		this->setBadThrow("426", "Upgrade Required");
	}
	this->petition.setProtocol(protocol);
	this->protocol = protocol;
}

void Response::handleHeaders(std::string headers)
{
	// SI AL FINAL SE TIENE QUE HACER ALGO CON LOS HEADERS PUES YA SE VERA
	int start;
	int end;
	if (((const unsigned long)(start = headers.find("Content-Length:"))) != std::string::npos)
	{
		start += 15;
		if (((const unsigned long)(end = headers.find("\r\n"))) != std::string::npos)
			this->setBadThrow("400", "Bad Request");
		std::string maxBodySize = this->server.getMaxSize();
		if (maxBodySize != "" && strToNum(headers.substr(start, end)) > strToNum(maxBodySize))
			this->setBadThrow("406", "Not Acceptable");
	}
	if (((const unsigned long)(start = headers.find("Content-Type:"))) != std::string::npos)
	{
		start += 13;
		if (((const unsigned long)(end = headers.find("\r\n"))) == std::string::npos)
			this->setBadThrow("400", "Bad Request");
		this->petition.setType(headers.substr(start, end));
	}
	this->petition.setHeaders(headers);
}

std::string Response::setResponseHead(std::string &resp)
{
	resp = "";
	resp.append(this->protocol);
	resp.append(" ");
	resp.append(this->statusCode);
	resp.append(" ");
	resp.append(this->statusMsg);
	resp.append("\r\n");
	if (!this->body.empty())
	{
		resp.append("Content-Type: ");
		resp.append(this->contentType);
		resp.append("\r\n");
		for (std::vector<std::string>::iterator it = this->heads.begin(); it != this->heads.end(); it++)
		{
			resp.append(*it);
			resp.append("\r\n");
		}
	}
	resp.append("\r\n");

	return (resp);
}

void Response::sendResponseMsg(int socketFd)
{
	// PARSE DEL PATH DEL ARXIU AMB access (que existexi i permisos)
	// OBRIR L'ARXIU I REBRE INFO AMB stat()
	// LLEGIR DEL ARXIU AMB UN BUFFER DE MAX_BODYSIZE (per fer nomes una lectura)
	// (si la stat_buf diu que el stat_buf.st_size es mes petit que el stat_buf.blksize nomes llegir blksize perque vol dir que el body es massa gran i nomes llegim lo possible)
	// WRITE DEL HEAD
	// WRITE DEL BUFFER AL SOCKET (de fet nomes tractar body si el statusCode es 200)
	// POTSER UTILITZAR SYNC I/O PER L'OPEN

	std::string method = this->petition.getMethod();
	std::string root = this->server.getRoot();
	char *path = const_cast<char *>(root.append(this->petition.getPath()).c_str());
	
	try
	{
		if (this->statusCode == "200")
		{
			if (method == "GET")
			{
				if (access(path, F_OK) < 0)
					this->setBadThrow("404", "Not Found");
				if (access(path, R_OK) < 0)
					this->setBadThrow("403", "Forbidden");
				this->doGet(path);
			}
			else if (method == "DELETE")
			{
				if (access(path, F_OK) < 0)
					this->setBadThrow("404", "Not Found");
				std::remove(path);
			}
			else if (method == "POST")
			{
				// EL POST SE TIENE QUE MEJORAR
				int fdPath;

				if ((fdPath = open(path, O_CREAT | O_NONBLOCK | O_EXCL)) < 0)
				{
					if (errno == EEXIST)
						this->setBadThrow("403", "Forbidden");
					this->setBadThrow("500", "Internal Server Error");
				}
				close(fdPath);
			}
		}
	}
	catch(const std::exception& e) { }

	std::string respMsg;

	this->setResponseHead(respMsg);
	std::cout << "RESPONSE:\n" << respMsg.c_str() << this->body;
	send(socketFd, respMsg.c_str(), respMsg.size(), 0);
	if (!this->body.empty())
		send(socketFd, this->body.c_str(), this->body.size(), 0);
}

void	Response::doGet(char *path)
{
	int fdPath;
	ssize_t readedSize;
	char buffer_body[MAX_BODYSIZE + 1];
	std::memset(buffer_body, '\0', MAX_BODYSIZE);
	struct stat stat_buf;
	
	if ((fdPath = open(path, O_RDONLY | O_NONBLOCK)) < 0)
		this->setBadThrow("500", "Internal Server Error");

	stat(path, &stat_buf);
	int tmp_statSize = stat_buf.st_size;
	while (tmp_statSize > 0)
	{
		if (tmp_statSize < stat_buf.st_blksize)
		{
			if ((readedSize = read(fdPath, buffer_body, tmp_statSize)) < 0)
				this->setBadThrow("500", "Internal Server Error");
		}		
		else
		{
			if ((readedSize = read(fdPath, buffer_body, stat_buf.st_blksize)) < 0)
				this->setBadThrow("500", "Internal Server Error");
		}
		tmp_statSize -= readedSize;
	}
	std::string tmpBody(buffer_body);
	this->body = tmpBody;
	close(fdPath);
}