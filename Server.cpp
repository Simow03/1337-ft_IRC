#include "Server.hpp"

Server::Server(int _port, std::string &_password) : port(_port), password(_password)
{
	try
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			throw std::runtime_error("socket() : system call error.");

		fcntl(sockfd, F_SETFL, O_NONBLOCK);

		int opt = 1;
    	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    		throw std::runtime_error("setsockopt() : system call error.");

		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = INADDR_ANY;
		sa.sin_port = htons(port);

		if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		{
			close(sockfd);
			throw std::runtime_error("bind() : system call error.");
		}

		if (listen(sockfd, BACKLOG) < 0)
		{
			close(sockfd);
			throw std::runtime_error("listen() : system call error.");
		}

		pollfds.fd = sockfd;
		pollfds.events = POLLIN;
		clientfds.push_back(pollfds);

		std::cout << YELLOW << BOLD << "\n\t. . . Waiting for connections . . .\n"
				  << RESET << std::endl;

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
};

void Server::runServer() {

	try
	{
		while (g_running)
		{
			pollStatus = poll(&clientfds[0], clientfds.size(), 100);
			if (pollStatus < 0)
			{
				for (size_t i = 0; i < clientfds.size(); i++)
					close(clientfds[i].fd);
	
				if (sockfd > 0)
					close(sockfd);
	
				clientfds.clear();

				if (!g_running) {
					std::cout << "\nSIGNIT RECEIVED ; QUIT SERVER" << std::endl;
					return ;
				}

				throw std::runtime_error("poll() : system call error.");
			}
			if(pollStatus == 0)
				continue ;
			for (size_t i = 0; i < clientfds.size(); i++)
			{
				if (clientfds[i].revents == 0)
					continue;
				if (clientfds[i].fd == sockfd && (clientfds[i].revents & POLLIN))
				{
						acceptNewConnection();
						continue;
				}
				if (clientfds[i].revents & POLLIN)
				{
					if (!receiveClientData(i)) {
						disconnectClient(i);
						i--;
					}
				}
				if (clientfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					disconnectClient(i);
					i--;
				}
			}
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void Server::acceptNewConnection() {
	do
	{
		ca_len = sizeof(ca);
		clientfd = accept(sockfd, (struct sockaddr *)&ca, &ca_len);

		if (clientfd < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			throw std::runtime_error("accept() : system call error.");
		}

		int flag = fcntl(clientfd, F_GETFL, 0);
		fcntl(clientfd, F_SETFL, flag | O_NONBLOCK);

		pollfds.fd = clientfd;
		pollfds.events = POLLIN;
		clientfds.push_back(pollfds);
		
		std::cout << GREEN << BOLD << "\nclient " << clientfd << " connected successfuly!\n" << RESET << std::endl;

		send(clientfd, GREEN BOLD "\nWelcome to the server!\n\n" RESET, 39, 0);

	} while (clientfd != -1);
	
}

bool Server::receiveClientData(size_t i) {

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	int fd = clientfds[i].fd;
	int received = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (received < 0)
		return false;
	
	buffer[received] = '\0';

	std::string command(buffer);

	std::cout << CYAN << "\nclient " << fd << ": " << RESET << command << std::endl;

	// need to process the type of command received
	return true;
}

void Server::disconnectClient(size_t i) {
	int fd = clientfds[i].fd;

	std::cout << RED << BOLD << "\nclient " << fd << " disconnected!\n" << RESET << std::endl;

	close(fd);

	clientfds.erase(clientfds.begin() + i);
}

Server::~Server() {

	for (size_t i = 0; i < clientfds.size(); i++)
		close(clientfds[i].fd);
	
	if (sockfd > 0)
		close(sockfd);
	
	clientfds.clear();
};