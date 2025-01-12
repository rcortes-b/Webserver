#include "../includes/Response.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerLocation.hpp"

static pid_t	childPS = -1;

static void	handleSignal(int sig)
{
	if (sig == SIGALRM && childPS > 0)
		kill(childPS, SIGKILL);
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

char	*doCgi (char *path)
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
		if (execve(path, full_path, NULL) < 0)
			throw ThrowError("Error: Failed executing CGI program");
	}
	else
	{
		childPS = pid;
		alarm(5);
		signal(SIGALRM, handleSignal);
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
		dup2(fd_in, STDIN_FILENO); //check if this is necesary
		close(fd_in); //check this out !!!!! no deberia estar mal !!!
		dup2(fd_out, STDOUT_FILENO); //check if this is necesary
		close(fd_out); //check this out !!!!! no deberia estar mal !!!
		if (status == SIGKILL)
			return NULL;
	}
	char *body_ptr = new char[body_content.size() + 1];
	std::strcpy(body_ptr, body_content.c_str());
	std::cout << "\n\n\n\n\nhere\n" << body_content << "\nhere\n\n\n\n\n" << std::endl;
	return body_ptr;
}
