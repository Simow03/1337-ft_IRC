NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp src/Server.cpp src/Client.cpp src/parse.cpp src/join.cpp src/mode.cpp src/Bot.cpp src/invite.cpp \
	src/kick.cpp src/topic.cpp src/prvmsg.cpp src/utils.cpp src/Channel.cpp

HEADERS = inc/Bot.hpp inc/Server.hpp inc/Client.hpp inc/parse.hpp inc/Channel.hpp inc/repl.hpp

OBJS = ${SRCS:.cpp=.o}

DEPS = ${HEADERS}

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

%.o : %.cpp ${DEPS}
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all

.PHONY: all clean fclean re