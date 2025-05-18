NAME = ircserv
BOT_NAME = ircbot

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp src/Server.cpp src/Client.cpp src/parse.cpp src/join.cpp src/mode.cpp src/invite.cpp \
	src/kick.cpp src/topic.cpp src/prvmsg.cpp src/utils.cpp src/Channel.cpp
BOT_SRCS = src/Bot.cpp src/BotMain.cpp

HEADERS = inc/Server.hpp inc/Client.hpp inc/parse.hpp inc/Channel.hpp inc/repl.hpp inc/Bot.hpp

OBJS = ${SRCS:.cpp=.o}
BOT_OBJS = ${BOT_SRCS:.cpp=.o}

DEPS = ${HEADERS}

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

%.o : %.cpp ${DEPS}
	${CXX} ${CXXFLAGS} -c $< -o $@

bot: ${BOT_NAME}

${BOT_NAME}: ${BOT_OBJS}
	${CXX} ${CXXFLAGS} ${BOT_OBJS} -o ${BOT_NAME}

clean:
	rm -f ${OBJS} ${BOT_OBJS}

fclean: clean
	rm -f ${NAME} ${BOT_NAME}

re: fclean all

.PHONY: all clean fclean re bot