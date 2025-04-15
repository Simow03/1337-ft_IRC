#include "Server.hpp"

Server::Server(int _port, std::string _password) : port(_port), password(_password) 
{
	try {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			throw std::exception("socket() : system call error.");

		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = INADDR_ANY;
		sa.sin_port = htons(this->port);

		if (bind(sockfd, (struct sockaddr_in *)&sa, sizeof(sa)) < 0) {
			close(sockfd);
			throw std::exception("bind() : system call error.");
		}

		if (listen(sockfd, BACKLOG) < 0) {
			close(sockfd);
			throw std::exception("listen() : system call error.");
		}

		pollfds.fd = sockfd;
		pollfds.events = POLLIN;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
};

Server::~Server(){};