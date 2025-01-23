#include "../includes/CGI.hpp"
#include "../includes/Response.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerLocation.hpp"

CGI::CGI(void)
{
	_pathAction = "./www/web3";
	_method = "GET";
	_envp = NULL;
	_is_cgi = false;
	_isActionForm = false;
}

CGI::CGI( CGI const &obj )
{
	if (this != &obj) {
		_strEnv = obj._strEnv;
		_method = obj._method;
		_pathAction = obj._pathAction;
		_envp = obj._envp;
		_is_cgi = obj._is_cgi;
		_isActionForm = obj._isActionForm;
	}
}

CGI	&CGI::operator=(CGI const &obj)
{
	if (this != &obj)
		*this = obj;
	return *this;
}

CGI::~CGI(void)
{
	if (_envp) {
		while (*_envp++)
			delete *_envp;
		delete _envp;
	}
}

void	CGI::setIsCgi(bool is_cgi)
{
	_is_cgi = is_cgi;
}

bool	CGI::getIsCgi(void)
{
	return _is_cgi;
}

bool	CGI::getIsActionForm(void)
{
	return _isActionForm;
}

std::string	CGI::getActionForm( void ) const
{
	return _pathAction;
}

void    CGI::getEnvironment(char *path)
{
	std::string strPath = path;
	int	pos = 0;
	for (size_t i = 0; i < strPath.size(); i++) {
		if (strPath[i] == '&') {
			_strEnv.push_back(strPath.substr(pos, i));
			pos = i + 1;
		}
		else if (i + 1 == strPath.size())
			_strEnv.push_back(strPath.substr(pos, i));
	}
}

void    CGI::parseEnvironment( void )
{
	if (_strEnv.empty())
		return ;
	_envp = new char*[_strEnv.size() + 1];
	int	counter = 0;
	for (std::vector<std::string>::iterator it = _strEnv.begin(); it != _strEnv.end(); it++) {
		_envp[counter] = new char[it->size() + 1];
		_envp[counter++] = const_cast<char *>((*it).c_str());
	}
	_envp[counter] = NULL;
}

void	CGI::check_for_action(std::ifstream &f)
{
	std::string	line;
	size_t			s_index = 0;
	size_t			e_index = 0;
	while (std::getline(f, line))
	{
		if (line.find("action=\"/cgi-bin/") != std::string::npos)
		{
			s_index = line.find('/');
			for (e_index = s_index; e_index < line.size() && !std::isspace(line[e_index]); e_index++);
			_pathAction += line.substr(s_index, e_index - s_index - 1);
			_isActionForm = true;
			return ;
		}
	}
}

static bool	path_has_extension(char *path, std::vector<std::string> cgi_ext)
{
	unsigned int vec_size = cgi_ext.size();
	unsigned int rr_counter = 0;
	unsigned int path_len = std::strlen(path);

	for (unsigned int i = 0; i < vec_size; i++) {
		rr_counter = 1;
		for (int len = cgi_ext[i].size() - 1; len >= 0; len--) {
			if (path[path_len - rr_counter] != cgi_ext[i][len])
				break ;
			rr_counter++;
			if (!len && path_len - rr_counter > 0 && path[path_len - rr_counter] == '.')
				return true;
		}
	}
	return false;
}

bool	is_cgi(Response &resp, char *path)
{
	std::vector<std::string> cgi_ext;
	cgi_ext.push_back("js");
	cgi_ext.push_back("py");
	cgi_ext.push_back("sh");
	cgi_ext.push_back("php");
	cgi_ext.push_back("c");
	ServerLocation	location = resp.getLocation();
	if (location.getCgiExtension().empty() || !path_has_extension(path, cgi_ext))
		return false;
	std::vector<std::string>::iterator it_beg = location.getCgiExtension().begin();
	std::vector<std::string>::iterator it_end = location.getCgiExtension().end();
	for (unsigned int i = 0; i < cgi_ext.size(); i++) {
		it_beg = location.getCgiExtension().begin();
		while (it_beg != it_end) {
			if (*it_beg == cgi_ext[i])
				return true;
			it_beg++;
		}
	}
	return false;
}

char	*CGI::doCgi (char *path)
{
	std::string body_content = "";
	pid_t	pid;
	int	fd_in = dup(STDIN_FILENO);
	int	fd_out = dup(STDOUT_FILENO);
	int fd[2];
	if (pipe(fd) == -1)
		throw ThrowError("Error: Error creating the pipe") ; //error
	pid = fork();
	if (!pid)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		char *full_path[] = {path, NULL};
		this->parseEnvironment();
		if (execve(path, full_path, _envp) < 0)
			throw ThrowError("Error: Failed executing CGI program");
	}
	else
	{
		close(fd[1]);
		int status;
		waitpid(pid, &status, 0);
		//read si el hijo ha finalizado el proceso correctamente
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			int			read_status = 0;
			char buff[2];
			while((read_status = read(fd[0], buff, 1)) > 0) {
				buff[1] = '\0';
				body_content += static_cast<std::string>(buff);
			}
			//delete hi;
			if (read_status == -1)
				throw ThrowError("Error: Error reading from the pipe");
		}
		close(fd[0]);
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
		if (status == SIGKILL)
			return NULL;
	}
	char *body_ptr = new char[body_content.size() + 1];
	std::strcpy(body_ptr, body_content.c_str());
	return body_ptr;
}
