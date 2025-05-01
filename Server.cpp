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

		ircBot = new Bot("IRC_BOT", "BOT !");

		std::cout << YELLOW << BOLD << "\n\t. . . Waiting for connections . . .\n"
				  << RESET << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
};

void Server::runServer()
{

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

				if (!g_running)
				{
					std::cout << "\nSIGNIT RECEIVED ; QUIT SERVER" << std::endl;
					return;
				}

				throw std::runtime_error("poll() : system call error.");
			}
			if (pollStatus == 0)
				continue;
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
					if (!receiveClientData(i))
					{
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
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void Server::acceptNewConnection()
{
	do
	{
		ca_len = sizeof(ca);
		clientfd = accept(sockfd, (struct sockaddr *)&ca, &ca_len);

		if (clientfd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			throw std::runtime_error("accept() : system call error.");
		}

		fcntl(clientfd, F_SETFL, O_NONBLOCK);

		pollfds.fd = clientfd;
		pollfds.events = POLLIN;
		clientfds.push_back(pollfds);

		std::cout << GREEN << BOLD << "\nclient " << clientfd << " connected successfuly!\n"
				  << RESET << std::endl;

		std::string authMssg = GREEN BOLD "\n\tWelcome to the server!\n\n" RESET "Please authenticate in this format : NICKNAME USERNAME PASSWORD\n" RESET;

		send(clientfd, authMssg.c_str(), authMssg.size(), 0);

	} while (clientfd != -1);
}

bool Server::receiveClientData(size_t i)
{

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	int fd = clientfds[i].fd;
	int received = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (received < 0)
		return false;

	buffer[received] = '\0';

	std::string command(buffer);

	while (!command.empty() && command.back() == '\n')
		command.pop_back();

	bool clientFound = false;
	Client *client = NULL;

	std::vector<Client>::iterator clientIt = clients.end();

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			clientFound = true;
			clientIt = it;
			client = &(*it);
			break;
		}
	}

	if (clientFound)
	{
		std::cout << CYAN << "\nclient " << fd << ": " << RESET << command << std::endl;

		if (ircBot->isBotCommand(command))
			ircBot->processCommand(command, *client, fd);
		// remove new line from variable buffer
		if(buffer[received - 1] == '\n')
			buffer[received - 1] = '\0';

		if (command == "QUIT")
			disconnectClient(i);

		parse handl(buffer, this, *client);

		return true;
	}

	else
		authenticateClient(command, fd);

	return true;
}

void Server::disconnectClient(size_t i)
{
	int fd = clientfds[i].fd;

	std::cout << RED << BOLD << "\nclient " << fd << " disconnected!\n"
			  << RESET << std::endl;
	
	std::string quitMssg = std::string(YELLOW) + std::string(BOLD) + "\n\t      You have been disconnected. GoodBye !\n\n" + std::string(RESET) ;
    
	send(fd, quitMssg.c_str(), quitMssg.size(), 0);

	Client* disconnectedClient = NULL;
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd) {
			disconnectedClient = &(*it);
			break;
		}
	}

	if (disconnectedClient != NULL) {
		for (size_t i = 0; i < channels.size(); i++) {
			if (channels[i].client_exist(*disconnectedClient)) {
				channels[i].remove_client(*disconnectedClient);
				if (channels[i].client_is_admin(*disconnectedClient))
					channels[i].remove_client_as_admin(*disconnectedClient);
			}
		}
	}

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd) {
			clients.erase(it);
			break;
		}
	}

	close(fd);
	clientfds.erase(clientfds.begin() + i);

}

void Server::authenticateClient(std::string &command, int fd)
{

	if (command.empty())
	{
		sendIRCReply(fd, ERR_NEEDMOREPARAMS, "ss", ":Not enough parameters");
		std::string errorMssg = RED BOLD "\nPlease authenticate in this format : NICKNAME USERNAME PASSWORD\n" RESET;
		send(fd, errorMssg.c_str(), errorMssg.size(), 0);
		return;
	}

	std::istringstream iss(command);
	std::string nickname, username, inPass;

	if (iss >> nickname >> username >> inPass)
	{
		bool unique = true;

		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->getNickName() == nickname)
			{
				unique = false;
				break;
			}
		}
		if (!unique)
		{
			sendIRCReply(fd, ERR_NICKNAMEINUSE, "ss", ":Nickname is already in use");
			std::string errorMssg = RED BOLD "\nNickname already in use. plase try again.\n" RESET;
			send(fd, errorMssg.c_str(), errorMssg.size(), 0);
			return;
		}
		if (inPass != password)
		{
			sendIRCReply(fd, ERR_PASSWDMISMATCH, "ss", ":Password incorrect");
			std::string errorMsg = RED BOLD "\nInvalid password. Please try again.\n" RESET;
			send(fd, errorMsg.c_str(), errorMsg.size(), 0);
			return;
		}

		std::string ipStr = inet_ntoa(ca.sin_addr);

		Client client(fd, ipStr, nickname, username, true);

		clients.push_back(client);

		sendIRCReply(fd, RPL_WELCOME, nickname, ":Welcome to the irc.localhost Network," + nickname + "[!" + username + "@" + ipStr + "]");
		std::string welcomeMsg = GREEN BOLD "\nAuthentication successful! Welcome, " + nickname + "!\n\n" RESET;
		send(fd, welcomeMsg.c_str(), welcomeMsg.size(), 0);
		std::cout << GREEN << BOLD << "\nclient " << fd << " authenticated successfuly!\n"
				  << RESET << std::endl;

		return;
	}
	else
	{
		sendIRCReply(fd, ERR_NEEDMOREPARAMS, "ss", ":Not enough parameters");
		std::string errorMsg = RED BOLD "\nInvalid format. Please use: NICKNAME USERNAME PASSWORD\n" RESET;
		send(fd, errorMsg.c_str(), errorMsg.size(), 0);
	}
}

void Server::sendIRCReply(int fd, const std::string& numeric, const std::string& target, const std::string& message){
	std::string reply = ":" + std::string("irc.localhost") + " " + numeric + " " + target + " " + message + "\r\n";
	send(fd, reply.c_str(), reply.length(), 0);
}


Server::~Server()
{

	for (size_t i = 0; i < clientfds.size(); i++)
		close(clientfds[i].fd);

	if (sockfd > 0)
		close(sockfd);

	clientfds.clear();

	delete ircBot;
};