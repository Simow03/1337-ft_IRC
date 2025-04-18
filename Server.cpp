#include "Server.hpp"

Server::Server(int _port, std::string &_password) : port(_port), password(_password)
{
	try
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			throw std::runtime_error("socket() : system call error.");

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

		timeout = -1;
		pollStatus = poll(&clientfds[0], clientfds.size(), timeout);
		if (pollStatus < 0)
		{
			// CleanUpFds();
			throw std::runtime_error("poll() : system call error.");
		}

		bool disconnected = false;

		for (size_t i = 0; i < clientfds.size(); i++)
		{
			if (clientfds[i].revents == 0)
				continue;
			if (clientfds[i].revents & POLLIN)
			{
				if (clientfds[i].fd == sockfd)
				{
					do
					{
						ca_len = sizeof(ca);
						clientfd = accept(sockfd, (struct sockaddr *)&ca, &ca_len);

						if (clientfd < 0)
						{
							throw std::runtime_error("accept() : system call error.");
							break;
						}

						// create the new client

						pollfds.fd = clientfd;
						pollfds.events = POLLIN;
						clientfds.push_back(pollfds);
						std::cout << GREEN << BOLD << "\nclient " << clientfds.back().fd << " connected successfully!\n"
								  << RESET << std::endl;
					} while (clientfd != -1);
				}
				else
					// serve the client commands
					break;
			}
			else
				disconnected = true;

			if (disconnected)
			{
				std::cerr << RED << BOLD << "\nclient " << clientfds[i].fd << " disconnected!\n" << RESET << std::endl;

				// use quit for the disconnected client

				close(clientfds[i].fd);
				clientfds.erase(clientfds.begin() + i);
				break;
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
};

Server::~Server() {};