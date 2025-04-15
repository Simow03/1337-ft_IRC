#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

class Server
{
private:
	int port;
	std::string password;
	struct sockaddr_in sa;
	struct pollfd pollfds;
	int sockfd;

public:
	Server(int port, std::string password);
	~Server();	
};


#endif