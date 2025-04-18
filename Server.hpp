#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <vector>

# define BACKLOG 1024

# define RED "\033[31m"
# define BOLD "\033[1m"
# define UNDERLINE "\033[4m"
# define RESET "\033[0m"
# define YELLOW "\033[33m"
# define GREEN "\033[32m"


class Server
{
private:
	int port;
	std::string password;
	struct sockaddr_in sa;
	struct sockaddr_in ca;
	socklen_t ca_len;
	struct pollfd pollfds;
	std::vector<struct pollfd> clientfds;
	int sockfd;
	int clientfd;
	int pollStatus;
	int timeout;

public:
	Server(int port, std::string& password);
	~Server();
};


#endif