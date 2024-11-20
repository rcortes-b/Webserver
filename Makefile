NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf

HEAD = includes/ServerConfig.hpp includes/ConfigFile.hpp \
		includes/SimpleSocket.hpp includes/Response.hpp \
		includes/Petition.hpp includes/ServerLocation.hpp \
		includes/ListeningSocket.hpp
SRCS = srcs/main.cpp srcs/ConfigFile.cpp srcs/CGI.cpp \
		srcs/ServerConfig.cpp srcs/ServerLocation.cpp \
		srcs/connections.cpp srcs/SimpleSocket.cpp \
		srcs/Response.cpp srcs/Petition.cpp \
		srcs/ListeningSocket.cpp \
		srcs/petitions_handler.cpp
    
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all : $(NAME)

%.o : %.cpp Makefile $(HEAD)
	$(CC) $(FLAGS) -MMD -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean :
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re : fclean all

-include $(DEPS)

.PHONY : all clean fclean re
