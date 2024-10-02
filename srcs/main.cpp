//include headers here
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerRequests.hpp"
#include "../includes/ConfigFile.hpp"

/*
ERROR MESSAGES
	- "The file has not a valid format. \".conf format is required.\""
*/
static int	err_code = 0;

class ThrowError : public std::exception {
	public:
			virtual const char	*what(unsigned int err_code) const throw() {
				switch (err_code) {
					case 0:
							return ("The file has not a valid format. \".conf format is required.\"");
					case 1:
							return ("Error opening the file.");
					case 2:
							return ("");
					case 3:
							return ("");
					default:
							return ("delete this. error code not update ERROR !!!");
				}
				return ("");
			}
};

static void	parse_file(std::string &file)
{
	size_t	len = file.size();
	if (len < 6) //Comprueba que tiene un mínimo de carácteres para que sea .conf
		throw ThrowError();
	std::string	format = ".conf";
	len -= 1;
	for (unsigned int i = 0; i < 5; i++) { //Se asegura que es un .conf comparando los últimos carácteres
		if (file[len] != format[4 - i])
			throw ThrowError();
		len--;
	}

	std::fstream	f;
	f.open(file.c_str());
	if (!f)
	{
		err_code = 1;
		throw ThrowError();
	}
	(void)parse_server_data(f);
}

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1] || !*argv[1])
	{
		std::cerr << "Invalid number of arguments or argument is invalid." << std::endl;
		return (1);
	}
	try {
		std::string file = argv[1];
		parse_file(file);
	} catch (ThrowError &e) {
		std::cerr << e.what(err_code) << std::endl;
	}
	//parse file
}