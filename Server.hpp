#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <unistd.h>
# include <vector>
# include <cstring>
# include <cerrno>
# include <signal.h>
# include "channel.hpp"
# include "parse.hpp"
#include "Client.hpp"

# define BACKLOG 1024
# define BUFFER_SIZE 1024
extern bool g_running;

# define RED "\033[31m"
# define BOLD "\033[1m"
# define UNDERLINE "\033[4m"
# define RESET "\033[0m"
# define YELLOW "\033[33m"
# define GREEN "\033[32m"
# define CYAN "\033[36m"

class Client;
class channel;

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
	std::vector<Client > clients;
	std::vector<channel> channels;
	int sockfd;
	int clientfd;
	int pollStatus;

public:
	Server(int port, std::string& password);
	~Server();

	void runServer(void);
	void acceptNewConnection(void);
	bool receiveClientData(size_t i);
	void disconnectClient(size_t i);
	void add_channel(std::string name,std::string key, int i) //-> add channel
	{
		channel c(name,key, i);
		channels.push_back(c);
	}
	// Client* get_client(int fd)
	// {
	// 	for(size_t i = 0; i < clients.size();i++)
	// 	{
	// 		if(clients[i].getFd() == fd)
	// 			return &clients[i];
	// 	}
	// 	return NULL;
	// }
	int channel_exist(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			std::cout << "channel name: " << channel_name <<"?" << std::endl;
			if(channels[i].GetName() == channel_name)
				return 1;
		}
		return 0;
	}
	int client_exist(std::string channel_name, Client *client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].client_exist(client))
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	void add_client_to_channel(std::string channel_name, Client *client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				channels[i].add_client(client);
				return;
			}
		}
	}
	int channel_has_key(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].GetKey() != "")
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	std ::string get_channel_key(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				return channels[i].GetKey();
			}
		}
		return "";
	}
	void remove_client_from_channels(Client *client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].client_exist(client))
			{
				channels[i].remove_client(client);
			}
		}
	}
};


#endif