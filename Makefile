NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
RM = rm -rf

HEAD = includes/ServerConfig.hpp includes/ServerRequests.hpp \
		includes/SimpleSocket.hpp
SRCS = srcs/main.cpp \
		srcs/ServerConfig.cpp srcs/ServerRequests.cpp \
		srcs/connections.cpp srcs/SimpleSocket.cpp
    
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