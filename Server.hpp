#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
#include <poll.h>
#include <unistd.h>

# define BACKLOG 1024

class Server
{
private:
	int port;
	std::string password;
	struct sockaddr_in sa;
	struct pollfd pollfds;
	int sockfd;

public:
	Server(int port, std::string& password);
	~Server();
};


#endif