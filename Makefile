NAME = ircserv

CXX = c++

CXXFLAGS = #-fsanitize=address -g #-Wall -Wextra -Werror -std=c++98

SRCS = main.cpp Server.cpp Client.cpp parse.cpp join.cpp mode.cpp Bot.cpp invite.cpp kick.cpp topic.cpp

HEADERS = Server.hpp Client.hpp parse.hpp join.hpp mode.hpp channel.hpp

OBJS = ${SRCS:.cpp=.o}

DEPS = ${HEADERS}

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

%.o : %.cpp ${DEPS}
	${CXX} ${CXXFLAGS}  -c $< -o $@

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all

.PHONY: all clean fclean re