# include "../includes/Response.hpp"
# include "../includes/ConfigFile.hpp"

static bool	isActionForm = false;

Response::Response()
{
	this->protocol = "HTTP/1.1";
	this->statusCode = "200";
	this->statusMsg = "OK";
	this->contentType = "text/plain";
	this->body = NULL;
	this->heads.push_back("Server: webserv");
	this->bodySize = 0;
}

Response::Response(ServerConfig &server)
{
	this->protocol = "HTTP/1.1";
	this->statusCode = "200";
	this->statusMsg = "OK";
	this->contentType = "text/plain";
	this->server = server;
	this->body = NULL;
	this->heads.push_back("Server: webserv");
	this->bodySize = 0;
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
	this->bodySize = other.bodySize;
	for (int i = 0; i < other.bodySize; i++)
	{
		if (i == 0)
		{
			if (this->body)
				delete[] this->body;
			this->body = new char [other.bodySize + 1];
		}
		this->body[i] = other.body[i];
	}
	
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
		this->bodySize = other.bodySize;
		for (int i = 0; i < other.bodySize; i++)
		{
			if (i == 0)
			{
				if (this->body)
					delete[] this->body;
				this->body = new char [other.bodySize];
			}
			this->body[i] = other.body[i];
		}
		
	}

	return (*this);
}

Response::~Response()
{
	if (this->body)
		delete[] this->body;
}

void	Response::setRedirectThrow(std::string host)
{
	this->protocol = "HTTP/1.1";
	this->statusCode = "308";
	this->statusMsg = "Permanent Redirect";
	this->heads.push_back("Location: " + host);
	this->heads.push_back("Connection: close");
	
	throw Redirect();
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
	std::cout << "errorPageSize: " << errorPageSize << '\n';
	for (int i = 0; i < errorPageSize - 1; i++)
	{
		std::cout << "this->statusCode: " << this->statusCode << " == " << "errorPage[i]: " << errorPage[i] << "    access(errorPage[errorPageSize - 1].c_str(), R_OK):___" << errorPage[errorPageSize - 1].c_str() << "___\n";
		if (this->statusCode == errorPage[i] && access(errorPage[errorPageSize - 1].c_str(), R_OK) == 0)
		{
			std::cout << "Found: " << path << '\n';
			path = errorPage[errorPageSize - 1];
			break;
		}
	}
	this->doGet(const_cast<char *>(path.c_str()));
	throw BadPetition();
}

void	Response::setUp(std::string header, char *bodyContent, ssize_t bodySize)
{
	int token;
	
	if (((unsigned long)(token = header.find("\r\n"))) == std::string::npos)
		this->setBadThrow("400", "Bad Request");
	std::string statusLine = header.substr(0, token + 2);
	
	int from;
	for (size_t i = 0; i < 3; i++)
	{
		from = 0;
		if (i == 0)
		{
			if (((unsigned long)(token = statusLine.find(" "))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");
			std::cout << statusLine << " separator " << statusLine.substr(from, token) << '\n';
			std::string method = statusLine.substr(from, token);
			this->handleMethod(method);
			statusLine = statusLine.substr(++token);
		}
		else if (i == 1)
		{
			if (((unsigned long)(token = statusLine.find(" "))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");

			std::string path = statusLine.substr(from, token);
			this->handlePath(path);
			if (this->petition.getMethod() == "GET" && !this->location.getCgiExtension().empty())
				throw CGI_Exception();
			statusLine = statusLine.substr(++token);
		}
		else if (i == 2)
		{
			if (((unsigned long)(token = statusLine.find("\r\n"))) == std::string::npos)
				this->setBadThrow("400", "Bad Request");

			std::string protocol = statusLine.substr(from, token);
			this->handleProtocol(protocol);
			token += 2;
			statusLine = statusLine.substr(token);
		}
	}
	if (((unsigned long)(from = header.find("\r\n"))) == std::string::npos)
			this->setBadThrow("400", "Bad Request");

	if (((unsigned long)(token = header.find("\r\n\r\n", from))) == std::string::npos)
		this->setBadThrow("400", "Bad Request");
	token += 4;

	std::string headers = header.substr(from, token - from);
	this->handleHeaders(headers);
	
	this->petition.setBodyContent(bodyContent);
	this->petition.setBodySize(bodySize);
}

void	Response::setLocation(void)
{
	std::string path = this->petition.getPath();
	size_t start = path.find("/");
	size_t end = path.find("/", start + 1);
	if (end == std::string::npos)
	{
		this->location.setRootSimple(this->server.getRoot());
		this->location.setIndexSimple(this->server.getIndex());
		this->location.setAutoIndex(this->server.getAutoIndex());
		return;
	}
	path = path.substr(start, end);
	std::vector<ServerLocation> serverLocations = this->server.getLocation();
	for (std::vector<ServerLocation>::iterator it = serverLocations.begin(); it < serverLocations.end(); it++)
	{
		std::string locationRoute = (*it).getRoute();
		if (path == locationRoute)
		{
			this->location = *it;
			if (!this->location.getRedirect().empty())
				this->setRedirectThrow(this->location.getRedirect());
			return;
		}
	}
	this->setBadThrow("404", "NotFound");
}

void	Response::handleMethod(std::string method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		this->setBadThrow("405", "Method Not Allowed");
	this->petition.setMethod(method);
}

void	Response::checkMethods(void)
{
	std::string petitionMethod = this->petition.getMethod();
	std::vector<std::string> methods = this->location.getMethods();

	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
	{
		if (petitionMethod == *it)
			return;
	}
	this->setBadThrow("405", "Method Not Allowed");
}

void Response::handlePath(std::string path)
{
	int len = path.length();

	if (len > 0 && path[len - 1] != '/' && path.find('.') == std::string::npos)
		path.append("/");
	this->petition.setPath(path);
	//Reset length of path after appending the /
	len = path.length();
	//Set location
	this->setLocation();
	if (path.find('?') != std::string::npos)
		return ;
	//Check for the alredy configured method
	if (!this->location.getMethods().empty())
		this->checkMethods();

	std::string method = this->petition.getMethod();
	if(len > 0 && path[len - 1] == '/')
	{
		//SI ES GET FAIG INDEX I AUTODINDEX
		if (method == "GET")
			this->handleIndexes(path);
		//SI ES DELETE I ES UNA LOCATION
		else if (method == "DELETE")
			this->setBadThrow("403", "Forbidden");
		else if (method == "POST")
			return;
	}
	// EN CAS DE QUE SIGUI POST I NO ACABI EN '/' (ES POT CAMBIAR)
	else 
	{
		// if (method == "GET" && !this->location.getCgiExtension().empty())
		// 	throw CGI_Exception();
		/*else */if (method == "POST" && !is_cgi(*this, (char*)path.c_str()))
			this->setBadThrow("403", "Forbidden");
	}
	//Reset length of path after appending the index
	len = path.length();


	if (len > 4 && path[len - 5] == '.' && path[len - 4] == 'h' && path[len - 3] == 't' && path[len - 2] == 'm' && path[len - 1] == 'l')
	{
		this->petition.setPath(path);
		this->contentType = "text/html";
	}
	else if ((len > 3 && path[len - 4] == '.' && path[len - 3] == 'j' && path[len - 2] == 'p' && path[len - 1] == 'g') || 
	(len > 4 && path[len - 5] == '.' && path[len - 4] == 'j' && path[len - 3] == 'p' && path[len - 2] == 'e' && path[len - 1] == 'g'))
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
	else if (len > 3 && path[len - 4] == '.' && path[len - 3] == 't' && path[len - 2] == 'x' && path[len - 1] == 't')
	{
		this->petition.setPath(path);
		this->contentType = "text/plain";
	}
	else if (len > 3 && path[len - 4] == '.' && path[len - 3] == 'i' && path[len - 2] == 'c' && path[len - 1] == 'o')
	{
		this->petition.setPath(path);
		this->contentType = "image/x-icon";
	}
	else if (!is_cgi(*this, (char*)path.c_str()))
		this->setBadThrow("415", "Unsuppported Media Type");
}

void Response::doAutoIndex(char *path)
{
	DIR *dir = NULL;
	struct dirent *entry;
	std::string locationPath = this->location.getRoute() + "/";
	std::string strBody = \
	"<!DOCTYPE html>\n<html lang=\"en\">\n\t<h1>Index of ";
	
	strBody.append(locationPath);
	strBody.append("</h1>\n\t<br></br>\n");

	dir = opendir(path);
	if (!dir)
		this->setBadThrow("500", "Internal Server Error");
	
	while ((entry = readdir(dir)) != NULL)
	{
		std::string tmp(entry->d_name);
		strBody.append("\t<a href=\"");
		strBody.append(locationPath + tmp);
		strBody.append("\">");
		strBody.append(locationPath + tmp);
		strBody.append("</a><br></br>");
	}
	
	strBody.append("</html>\n");
	this->bodySize = strBody.size();
	this->body = new char[this->bodySize + 1];
	std::strcpy(this->body, strBody.c_str());
	this->contentType = "text/html";

	closedir(dir);
}

void Response::handleIndexes(std::string &path)
{
	std::string index = "index.html";
	std::string locationIndex = this->location.getIndex();
	
	//Is index
	if (!locationIndex.empty())
		index = locationIndex;
	//Is Autoindex
	else if (this->location.getAutoIndex() == ON)
		throw AutoIndex();
	path.append(index);
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
	int start;
	int end;
	if (((unsigned long)(start = headers.find("Content-Length:"))) != std::string::npos)
	{
		std::string found = "Content-Length:";
		start += found.length();
		while (std::isspace(headers[start++]));
		start--;
		if (((unsigned long)(end = headers.find("\r\n", start))) == std::string::npos)
			this->setBadThrow("411", "Length Required");
		std::string maxBodySize = this->server.getMaxSize();
		std::cout << "Content-Length: " << strToulNum(headers.substr(start, end - start)) << "   strToulNum(maxBodySize): " << strToulNum(maxBodySize) << '\n';
		if (maxBodySize != "" && strToulNum(headers.substr(start, end - start)) > strToulNum(maxBodySize))
			this->setBadThrow("413", "Content Too Large");
	}
	if (((unsigned long)(start = headers.find("Content-Type:"))) != std::string::npos)
	{
		std::string found = "Content-Type:";
		start += found.length();
		while (std::isspace(headers[start++]));
		start--;
		if (((unsigned long)(end = headers.find("\r\n", start))) == std::string::npos)
			this->setBadThrow("400", "Bad Request");
		this->petition.setType(headers.substr(start, end - start));
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
	if (this->body)
	{
		resp.append("Content-Type: ");
		resp.append(this->contentType);
		resp.append("\r\n");
	}
	for (std::vector<std::string>::iterator it = this->heads.begin(); it != this->heads.end(); it++)
	{
		resp.append(*it);
		resp.append("\r\n");
	}
	resp.append("\r\n");

	return (resp);
}
#include <stdlib.h>
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
	std::string root;
	if (this->location.getRoot().empty())
		root = this->server.getRoot();
	else
		root = this->location.getRoot();

	std::string strPath = this->petition.getPath();
	char *path = const_cast<char *>(root.append(strPath).c_str());
	
	try
	{
		std::cout << "StatusCode: " << statusCode << "\n" << "Method: " << method << '\n';
		if (this->statusCode == "200")
		{
			if (method == "GET")
			{
				if (std::strchr(path, '?'))
				{
					this->contentType = "text/html";
					size_t len = std::strlen(path);
					_cgi.getEnvironment(std::strchr(path, '?') + 1);
					while (path[--len] != '?')
						path[len] = '\0';
					path[len] = '\0';
				}
				if (access(path, F_OK) < 0)
					this->setBadThrow("404", "Not Found");
				if (access(path, R_OK) < 0)
					this->setBadThrow("403", "Forbidden");
				if (std::strlen(path) > 5 && !std::strcmp(&path[std::strlen(path) - 5], ".html")) {
					std::ifstream f(path);
					if (!f.is_open())
						this->setBadThrow("500", "Error connecting to the requested page");
					this->_cgi.check_for_action(f);
					if (this->_cgi.getIsActionForm())
						isActionForm = true;
					f.close();
				}
				else
					_cgi.setIsCgi(is_cgi(*this, path));
				if (strPath[strPath.length() - 1] == '/')
					this->doAutoIndex(path);
				else if (this->_cgi.getIsCgi()) //to review
				{
					std::cout << "FOUND CGI!!!" << '\n';
					this->body = this->_cgi.doCgi(path);
					if (isActionForm)
						isActionForm = false;
					if (!this->body) 
						this->setBadThrow("408", "Request Timeout");
					this->bodySize = std::strlen(body);
				}
				else
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
				std::cerr << "SIONO: " << this->petition.getBodyContent() <<"\n";
				if (access(path, F_OK) < 0)
					this->setBadThrow("404", "Not Found");
				if (is_cgi(*this, path)) {
					char pathi[this->petition.getBodySize() + 1];
					pathi[this->petition.getBodySize()] = '\0';
					std::memcpy(pathi, this->petition.getBodyContent(),this->petition.getBodySize());
					_cgi.getEnvironment(pathi);
					this->body = _cgi.doCgi(path);
					this->bodySize = std::strlen(this->body);
					this->contentType = "text/html";
				}
				else {
					std::string newPath(path);
					this->setFileName(newPath);
					path = const_cast<char *>(newPath.c_str());
					this->statusCode = "201";
					this->statusMsg = "Created";
					std::ofstream pathFile(path, std::ios::binary | std::ios::trunc);
					this->doPost(pathFile);
				}
			}
		}
	}
	catch(const std::exception& e) { }
	std::string respMsg;
	this->setResponseHead(respMsg);
	std::cout << "RESPONSE:\n" << respMsg.c_str();
	if (this->body)
		std::cout << this->body;
	std::cout << '\n';
	if (send(socketFd, respMsg.c_str(), respMsg.size(), MSG_NOSIGNAL) < 0)
		return;
	if (this->body)
	{
		if (send(socketFd, this->body, this->bodySize, MSG_NOSIGNAL) < 0)
			return;
	}
}

void	Response::doGet(char *path)
{
	std::ifstream pathFile(path, std::ios::binary | std::ios::ate);
	if (!pathFile.is_open())
		this->setBadThrow("500", "Internal Server Error");
	
	this->bodySize = static_cast<int>(pathFile.tellg());
	
	this->body = new char [this->bodySize + 1];
	std::memset(this->body, '\0', this->bodySize + 1);

	pathFile.seekg(0, std::ios::beg);
	pathFile.read(this->body, this->bodySize);
	
	pathFile.close();
}

void	Response::setFileName(std::string &newPath)
{
	unsigned long start;
	unsigned long end;
	std::string petitionType = this->petition.getType();
	std::string headers = this->petition.getHeaders();
	std::string fileName = "default";
	if (isActionForm)
		return ;
	if ((start = headers.find("Filename:")) != std::string::npos)
	{
		start += 9;
		while (std::isspace(headers[start++]));
		start--;
		
		if ((end = headers.find("\r\n", start)) == std::string::npos)
			this->setBadThrow("400", "Bad Request");
		fileName = headers.substr(start, end - start);
	}

	if (petitionType == "text/html")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".html");
	}
	else if (petitionType == "image/jpeg")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".jpg");
	}
	else if (petitionType == "text/javascript")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".js");
	}
	else if (petitionType == "text/css")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".css");
	}
	else if (petitionType == "image/x-icon")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".ico");
	}
	else if (petitionType == "text/plain")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".txt");
	}
	else if (petitionType == "plain/text")
	{
		if (fileName.find(".") == std::string::npos)
			fileName.append(".txt");
	}
	else
		this->setBadThrow("415", "Unsuppported Media Type");
	newPath.append(fileName);
	
}

void	Response::doPost(std::ofstream &pathFile)
{
	if (!pathFile.is_open())
		this->setBadThrow("500", "Internal Server Error");
	
	char *bodyPetition = this->petition.getBodyContent();
	if (bodyPetition)
	{
		pathFile.write(bodyPetition, this->petition.getBodySize());
		if (pathFile.fail())
			this->setBadThrow("500", "Internal Server Error");
	
		std::cout << "BODY_SIZE: " << this->petition.getBodySize() << '\n';
	}

	pathFile.close();
}

ServerLocation	Response::getLocation(void)
{
	return location;
}