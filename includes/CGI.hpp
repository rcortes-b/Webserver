#ifndef CGI_HPP
# define CGI_HPP

#include "ConfigFile.hpp"
#include <map>
#include <ctime>
class CGI {
    private:
            std::string _method;
            std::vector<std::string>	_strEnv;
            std::string	_pathAction;
            char	**_envp;
            bool	_is_cgi;
			bool	_isActionForm;
    public:
            CGI( void );
            CGI( CGI const &obj );
            CGI &operator=( CGI const &obj );
            ~CGI ( void );

			void	setIsCgi( bool is_cgi );
			bool	getIsCgi( void );
			bool	getIsActionForm( void );
			std::string	getActionForm( void ) const;
            char	*doCgi( char *path );
			void	check_for_action( std::ifstream &f );
            void    getEnvironment(char *path);
            void    parseEnvironment( void );
};

#endif