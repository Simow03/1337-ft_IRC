#include "../inc/Server.hpp"

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

		ircBot = new Bot("IRC_BOT", "BOT ");

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
					disconnectClient(i);

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
					int fd = clientfds[i].fd;
					if (newlyConnectedClients.find(fd) != newlyConnectedClients.end())
					{
						newlyConnectedClients.erase(fd);
						continue;
					}

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

		newlyConnectedClients.insert(clientfd);

		std::cout << GREEN << BOLD << "\nclient " << clientfd << " connected successfuly!\n"
				  << RESET << std::endl;

	} while (clientfd != -1);
}

bool Server::receiveClientData(size_t i)
{

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	int fd = clientfds[i].fd;
	int received = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (received <= 0)
	{
		if (received == 0)
			return false;

		if (errno == EWOULDBLOCK || errno == EAGAIN)
		{
			return true;
		}

		return false;
	}

	buffer[received] = '\0';

	std::string message(buffer);
	size_t pos = 0;
	std::string command;

	while ((pos = message.find("\r\n")) != std::string::npos || (pos = message.find("\n")) != std::string::npos)
	{
		command = message.substr(0, pos);
		message.erase(0, pos + (message[pos] == '\r' ? 2 : 1));
		if (!command.empty())
		{
			processCommand(command, fd, i);
		}
	}

	if (!message.empty())
	{
		processCommand(message, fd, i);
	}

	return true;
}

void Server::processCommand(std::string &command, int fd, size_t i)
{
	std::cout << CYAN << "\nclient " << fd << ": " << RESET << command << std::endl;

	std::stringstream iss(command);
	std::string cmd;
	iss >> cmd;

	bool clientFound = false;
	Client *client = NULL;

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			clientFound = true;
			client = &(*it);
			break;
		}
	}

	if (!clientFound && cmd != "QUIT")
	{
		std::string ipStr = inet_ntoa(ca.sin_addr);
		std::string tempNickname = "";
		std::string tempUsername = "";
		Client newClient(fd, ipStr, tempNickname, tempUsername, false, false);
		clients.push_back(newClient);
		client = &clients.back();
		clientFound = true;
	}

	if (cmd == "QUIT")
	{
		disconnectClient(i);
		return;
	}

	if (cmd == "PASS")
	{
		std::string inPass;
		if (iss >> inPass)
		{
			if (!inPass.empty() && inPass[0] == ':')
				inPass = inPass.substr(1);

			if (inPass == password)
				client->setHasPass(true);

			else
			{
				std::vector<std::string> params;
				params.push_back("Password incorrect");
				sendNumericReply(fd, ERR_PASSWDMISMATCH_N, client->getNickName().empty() ? "*" : client->getNickName(), params);
				return;
			}
		}
		else
		{
			std::vector<std::string> params;
			params.push_back("PASS");
			params.push_back("Not enough parameters");
			sendNumericReply(fd, ERR_NEEDMOREPARAMS_N, client->getNickName().empty() ? "*" : client->getNickName(), params);
		}
		return;
	}

	if (!client->getHasPass())
	{
		std::vector<std::string> params;
		params.push_back(cmd);
		params.push_back("You have not registered");
		sendNumericReply(fd, ERR_NOTREGISTERED_N, client->getNickName().empty() ? "*" : client->getNickName(), params);
		return;
	}

	if (cmd == "NICK")
	{
		std::string nickname;
		iss >> nickname;

		std::cout << nickname << std::endl;

		if (nickname.empty() || nickname == ":")
		{
			std::vector<std::string> errParams;
			errParams.push_back("No nickname given");
			sendNumericReply(fd, ERR_NONICKNAMEGIVEN_N, client->getNickName().empty() ? "*" : client->getNickName(), errParams);
			return;
		}

		bool nickUsedByAuthedUser = false;

		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->getNickName() == nickname && it->getFd() != fd && it->getIsAuthenticated())
			{
				nickUsedByAuthedUser = true;
				break;
			}
		}

		if (nickUsedByAuthedUser)
		{
			std::vector<std::string> errParams;
			errParams.push_back(nickname);
			errParams.push_back("Nickname is already in use");
			sendNumericReply(fd, ERR_NICKNAMEINUSE_N, client->getNickName().empty() ? "*" : client->getNickName(), errParams);
			return;
		}

		std::string oldNickname = clientFound ? client->getNickName() : "";

		if (clientFound)
		{
			std::vector<std::string> params;

			client->setNickName(nickname);

			if (!oldNickname.empty())
			{
				std::string nickChangeMsg = ":" + oldNickname + "!" + client->getUserName() + "@" + client->getIpAddress() + " NICK :" + nickname + "\r\n";

				std::vector<std::string> client_channels = get_channels(*client);
				for (size_t i = 0; i < client_channels.size(); i++)
				{
					std::vector<Client *> members = get_clients_in_channel(client_channels[i]);
					for (size_t i = 0; i < members.size(); ++i)
					{
						members[i]->sendMessage(nickChangeMsg);
					}
				}

				send(fd, nickChangeMsg.c_str(), nickChangeMsg.size(), 0);
			}

			if (client->getHasPass() && !client->getNickName().empty() && !client->getUserName().empty() && !client->getIsAuthenticated())
			{

				for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
				{
					if (it->getFd() != fd && it->getNickName() == nickname && !it->getIsAuthenticated())
					{
						std::vector<std::string> errParams;
						errParams.push_back(nickname);
						errParams.push_back("Nickname is already in use");
						sendNumericReply(it->getFd(), ERR_NICKNAMEINUSE_N, it->getNickName().empty() ? "*" : it->getNickName(), errParams);
						std::string tempNickName = "";
						it->setNickName(tempNickName);
					}
				}

				client->setIsAuthenticated(true);

				if (newlyConnectedClients.find(fd) != newlyConnectedClients.end())
					newlyConnectedClients.erase(fd);

				sendWelcomeMessage(fd, nickname);

				std::cout << GREEN << BOLD << "\nclient " << fd << " authenticated successfuly!\n"
						  << RESET << std::endl;
			}
		}
	}
	else if (cmd == "USER")
	{
		std::string username;
		iss >> username;

		if (username.empty())
		{
			std::vector<std::string> errParams;
			errParams.push_back("USER");
			errParams.push_back("Not enough parameters");
			sendNumericReply(fd, ERR_NEEDMOREPARAMS_N, client->getNickName().empty() ? "*" : client->getNickName(), errParams);
			return;
		}

		client->setUserName(username);

		if (client->getHasPass() && !client->getNickName().empty() && !client->getUserName().empty() && !client->getIsAuthenticated())
		{
			for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
			{
				if (it->getFd() != fd && it->getNickName() == client->getNickName() && !it->getIsAuthenticated())
				{
					std::vector<std::string> errParams;
					errParams.push_back(client->getNickName());
					errParams.push_back("Nickname is already in use");
					sendNumericReply(it->getFd(), ERR_NICKNAMEINUSE_N, it->getNickName().empty() ? "*" : it->getNickName(), errParams);
					std::string tempNickName = "";
					it->setNickName(tempNickName);
				}
			}

			client->setIsAuthenticated(true);

			if (newlyConnectedClients.find(fd) != newlyConnectedClients.end())
				newlyConnectedClients.erase(fd);

			sendWelcomeMessage(fd, client->getNickName());
		}
	}

	else if (cmd == "PING" || cmd == "PONG")
	{
		std::string extraParam;
		iss >> extraParam;

		std::vector<std::string> toPushParams;
		toPushParams.push_back(extraParam.empty() ? "IRC.localhost" : extraParam);
		sendIRCReply(fd, "IRC.localhost", "PONG", toPushParams);
	}

	else
	{
		if (!client->getIsAuthenticated())
		{
			std::vector<std::string> params;
			params.push_back(cmd);
			params.push_back("You must complete registration first");
			sendNumericReply(fd, ERR_NOTREGISTERED_N, client->getNickName().empty() ? "*" : client->getNickName(), params);
			return;
		}

		else
		{

			if (ircBot->isBotCommand(command))
			{
				ircBot->processCommand(command, *client, fd);
				return;
			}

			parse handl(command, this, *client);
		}
	}
}

void Server::disconnectClient(size_t i)
{
	int fd = clientfds[i].fd;

	std::cout << RED << BOLD << "\nclient " << fd << " disconnected!\n"
			  << RESET << std::endl;

	std::string quitMssg = std::string(YELLOW) + std::string(BOLD) + "\n\t      You have been disconnected. GoodBye !\n\n" + std::string(RESET);

	send(fd, quitMssg.c_str(), quitMssg.size(), 0);

	Client *disconnectedClient = NULL;
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			disconnectedClient = &(*it);
			break;
		}
	}

	if (disconnectedClient != NULL)
	{
		for (size_t i = 0; i < channels.size(); i++)
		{
			if (channels[i].client_exist(*disconnectedClient))
			{
				channels[i].remove_client(*disconnectedClient);
				if (channels[i].client_is_admin(*disconnectedClient))
					channels[i].remove_client_as_admin(*disconnectedClient);
			}
			std::string prefix = ":" + disconnectedClient->getNickName() + "!" + disconnectedClient->getUserName() + "@" + disconnectedClient->getIpAddress();
			std::string msg = prefix + " QUIT " + channels[i].GetName() + "\r\n";

			std::vector<Client *> channelClients = get_clients_in_channel(channels[i].GetName());
			for (std::vector<Client *>::iterator it = channelClients.begin(); it != channelClients.end(); ++it)
			{
				(*it)->sendMessage(msg);
			}
			std::vector<Client *> members = get_clients_in_channel(channels[i].GetName());
			std::string client_list = "353 " + disconnectedClient->getNickName() + " = " + channels[i].GetName() + " :";
			for (size_t i = 0; i < members.size(); ++i)
			{
				client_list += members[i]->getNickName() + " ";
			}
			client_list += "\r\n";
			disconnectedClient->sendMessage(client_list);
			std::string end_list = "366 " + disconnectedClient->getNickName() + " " + channels[i].GetName() + " :End of NAMES list\r\n";
			disconnectedClient->sendMessage(end_list);
		}
	}

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			clients.erase(it);
			break;
		}
	}

	close(fd);
	clientfds.erase(clientfds.begin() + i);
}

void Server::sendIRCReply(int fd, const std::string &prefix, const std::string &command, const std::vector<std::string> &params)
{
	std::string message = prefix.empty() ? "" : ":" + prefix + " ";
	message += command;

	for (size_t i = 0; i < params.size(); ++i)
	{
		if (i == params.size() - 1 && params[i].find(' ') != std::string::npos)
		{
			message += " :" + params[i];
		}
		else
		{
			message += " " + params[i];
		}
	}

	message += "\r\n";
	send(fd, message.c_str(), message.length(), 0);
}

void Server::sendNumericReply(int fd, const std::string &numeric, const std::string &nickname, const std::vector<std::string> &params)
{
	std::vector<std::string> newParams;
	newParams.push_back(nickname);

	for (size_t i = 0; i < params.size(); ++i)
	{
		newParams.push_back(params[i]);
	}
	sendIRCReply(fd, "IRC.localhost", numeric, newParams);
}

void Server::sendWelcomeMessage(int fd, std::string nickname)
{
	std::vector<std::string> params;

	params.push_back("Welcome to the IRC Network " + nickname + "!");
	sendNumericReply(fd, RPL_WELCOME_N, nickname, params);
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