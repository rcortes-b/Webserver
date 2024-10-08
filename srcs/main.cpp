//include headers here
#include "../includes/ServerConfig.hpp"
#include "../includes/ServerRequests.hpp"

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
							return ("");
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

void	parse_file(std::string file)
{
	//En este bloque se comprueba que sea del formato correcto (.conf)
	size_t	len = file.size();
	if (len < 6)
		throw ThrowError();
	std::string	format = ".conf";
	len -= 1;
	for (unsigned int i = 0; i < 5; i++) {
		if (file[len] != format[len])
			throw ThrowError();
		len--;
	}

	//En este bloque se comprueba 
	

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
	try
	{
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	//start connections

}