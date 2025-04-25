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
# include "Client.hpp"
# include "Bot.hpp"

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
class Bot;

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
	Bot *ircBot;

public:
	Server(int port, std::string& password);
	~Server();

	void runServer(void);
	void acceptNewConnection(void);
	bool receiveClientData(size_t i);
	void disconnectClient(size_t i);
	void authenticateClient(std::string& command, int fd);
	void add_channel(std::string name) //-> add channel
	{
		channel c(name);
		channels.push_back(c);
	}
	int channel_exist(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
				return 1;
		}
		return 0;
	}
	int client_exist(std::string channel_name, Client &client)
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
	void add_client_to_channel(std::string channel_name, Client &client)
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
				if(channels[i].is_private == 1)
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
	void remove_client_from_channels(Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].client_exist(client))
			{
				channels[i].remove_client(client);
			}
		}
	}
	int channel_need_inv(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].is_invited == 1)
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	int is_invitd(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].if_invited(client))
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	void add_client_as_channel_admin(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				channels[i].add_client_as_admin(client);
				return;
			}
		}
	}
	void set_channel_invite(std::string channel_name, int i)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].is_invited = i;
				return;
			}
		}
	}
	void set_channel_topic(std::string channel_name, int i)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].topic_restricted = i;
				return;
			}
		}
	}
	int client_isAdmin(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].client_is_admin(client))
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	void set_channel_limit(std::string channel_name, int limit)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].SetLimit(limit);
				channels[j].is_limited = 1;
				return;
			}
		}
	}
	void remove_channel_limit(std::string channel_name)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].is_limited = 0;
				return;
			}
		}
	}
	void set_channel_key(std::string channel_name, std::string key)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].is_private = 1;
				channels[j].Setkey(key);
				return;
			}
		}
	}
	void remove_channel_key(std::string channel_name)
	{
		for(size_t j = 0; j < channels.size();j++)
		{
			if(channels[j].GetName() == channel_name)
			{
				channels[j].is_private = 0;
				return;
			}
		}
	}
	Client *GetClientInChannel(std::string channel_name, std::string client_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				return channels[i].GetClient(client_name);
			}
		}
		return NULL;
	}
	int clientAdmin(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				if(channels[i].client_is_admin(client))
					return 1;
				else
					return 0;
			}
		}
		return 0;
	}
	void remove_client_as_channel_admin(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				channels[i].remove_client_as_admin(client);
				return;
			}
		}
	}
	int limit(std::string channel_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				int limit = channels[i].GetMaxUsers();
				int size = channels[i].get_size();
				if(channels[i].is_limited == 1)
				{
					if(size >= limit)
						return 1;
					return 0;
				}
				return 0;
			}
		}
		return 0;
	}
	int client_exist_by_name(std::string channel_name, std::string client_name)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				Client *temp = channels[i].GetClient(client_name);
				if (temp == NULL)
					return 0;
				else
					return 1;
			}
		}
		return 0;
	}
	int client_in_server(std::string client_name)
	{
		for(size_t i = 0; i < clients.size();i++)
		{
			if(clients[i].getNickName() == client_name)
				return 1;
		}
		return 0;
	}
	void add_client_as_invited(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				channels[i].add_client_as_invited(client);
				return;
			}
		}
	}
	Client *GetClientInServer(std::string client_name)
	{
		for(size_t i = 0; i < clients.size();i++)
		{
			if(clients[i].getNickName() == client_name)
				return &clients[i];
		}
		return NULL;
	}
	void remove_client_from_channel(std::string channel_name, Client &client)
	{
		for(size_t i = 0; i < channels.size();i++)
		{
			if(channels[i].GetName() == channel_name)
			{
				channels[i].remove_client(client);
				return;
			}
		}
	}

};


#endif