#include "../includes/Response.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerLocation.hpp"

static bool	path_has_extension(char *path, std::string cgi_ext[])
{
	unsigned int vec_size = cgi_ext->size();
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
	std::string cgi_ext[5] = {"js", "py", "sh", "php", "c"};
	ServerLocation	location = resp.getLocation();
	if (location.getCgiExtension().empty() || !path_has_extension(path, cgi_ext))
		return false;
	std::vector<std::string>::iterator it_beg = location.getCgiExtension().begin();
	std::vector<std::string>::iterator it_end = location.getCgiExtension().end();
	for (unsigned int i = 0; i < cgi_ext->size(); i++) {
		it_beg = location.getCgiExtension().begin();
		while (it_beg != it_end) {
			if (*it_beg == cgi_ext[i])
				return true;
			it_beg++;
		}
	}
	return false;
}

std::string	doCgi (char *path)
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
		close(fd[1]);
		std::string	body = "";
		int status;
		waitpid(pid, &status, 0);
		//read si el hijo ha finalizado el proceso correctamente
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			std::string tmp_content = "";
			int			read_status = 0;
			while((read_status = read(fd[0], (char *)tmp_content.c_str(), 1)) > 0)
				body_content += tmp_content;
			if (read_status == -1)
				throw ThrowError("Error: Error reading from the pipe");
		}
			// while (read fd[0]...)

		close(fd[0]);
		dup2(fd_in, STDIN_FILENO); //check if this is necesary
		close(fd_in); //check this out !!!!! no deberia estar mal !!!
		dup2(fd_out, STDOUT_FILENO); //check if this is necesary
		close(fd_out); //check this out !!!!! no deberia estar mal !!!
	}
	return body_content;
}
