#include "../inc/parse.hpp"


void brodcast_message(std::string channel_name, Server *server, std::string msg)
{
	std::vector<Client *> members = server->get_clients_in_channel(channel_name);
	for (size_t i = 0; i < members.size(); ++i)
	{
		members[i]->sendMessage(msg);
	}
}

void parse::execute_mode(std::string arg, Server *server, Client &client)
{
	std ::string channel_name;
	std::string mode;
	int mode_sign = 1;
	size_t pos = 2;

	if(arg.empty())
	{
		client.sendMessage("461 MODE :Not enough parameters\r\n");
		return;
	}
	std::vector<std::string> partsCmd = splitFct(arg);
	if(partsCmd.empty() || partsCmd.size() < 2)
	{
		client.sendMessage("461 MODE :Not enough parameters\r\n");
		return;
	}
	channel_name = partsCmd[0];
	if(!server->channel_exist(partsCmd[0]))
	{
		client.sendMessage("403 " + channel_name + " :No such channel\r\n");
		return;
	}
	mode = partsCmd[1];
	if(server->client_exist(channel_name, client) == 0)
	{
		client.sendMessage("442 " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	if(server->client_isAdmin(channel_name, client) == 0)
	{
		client.sendMessage("482 " + channel_name + " :You're not channel operator\r\n");
		return;
	}
	for(size_t i = 0; i < mode.size(); i++)
	{
		if(mode[i] == '+')
			mode_sign = 1;
		else if(mode[i] == '-')
			mode_sign = -1;
		else if(mode[i] == 'i')
		{
			if(mode_sign == 1)
			{
				server->set_channel_invite(channel_name, 1);
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " +i\r\n";
				brodcast_message(channel_name, server, message);
			}
			else
			{
				server->set_channel_invite(channel_name, 0);
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " -i\r\n";
				brodcast_message(channel_name, server, message);
			}
		}
		else if(mode[i] == 'o')
		{
			if (mode_sign == 1)
			{
				if(partsCmd.size() > pos)
				{
					std::string client_name = partsCmd[pos];
					int fd = server->GetClientInChannel(channel_name, client_name);
					if (fd == -1)
					{
						client.sendMessage("441 " + client_name + " " + channel_name + " :They aren't on that channel\r\n");
						pos++;
						continue;
					}
					Client *temp = NULL;
					for (std::vector<Client>::iterator it = server->clients.begin(); it != server->clients.end(); ++it)
					{
						if (it->getFd() == fd)
						{
							temp = &(*it);
							break;
						}
					}
					if (temp == NULL)
					{
						client.sendMessage("401 " + client_name + " :No such nick/channel\r\n");
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, *temp) == 1)
					{
						client.sendMessage("491 " + client_name + " :They are already channel operator\r\n");
						pos++;
						continue;
					}
					server->add_client_as_channel_admin(channel_name, *temp);
					std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " +o " + client_name + "\r\n";
					brodcast_message(channel_name, server, message);
					pos++;
				}
				else
				{
					client.sendMessage("461 MODE o :Operator mode requires a nickname\r\n");
					continue;
				}
			}
			else
			{
				if(partsCmd.size() > pos)
				{
					std::string client_name = partsCmd[pos];
					int fd = server->GetClientInChannel(channel_name, client_name);
					if (fd == -1)
					{
						client.sendMessage("441 " + client_name + " " + channel_name + " :They aren't on that channel\r\n");
						pos++;
						continue;
					}
					Client *temp = NULL;
					for (std::vector<Client>::iterator it = server->clients.begin(); it != server->clients.end(); ++it)
					{
						if (it->getFd() == fd)
						{
							temp = &(*it);
							break;
						}
					}
					if (temp == NULL)
					{
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, *temp) == 0)
					{
						pos++;
						continue;
					}
					server->remove_client_as_channel_admin(channel_name, *temp);
					std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " -o " + client_name + "\r\n";
					brodcast_message(channel_name, server, message);
					pos++;
				}
			}
		}
		else if(mode[i] == 't')
		{
			if(mode_sign == 1)
			{
				server->set_channel_topic(channel_name, 1);
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " +t\r\n";
				brodcast_message(channel_name, server, message);
			}
			else
			{
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " -t\r\n";
				brodcast_message(channel_name, server, message);
				server->set_channel_topic(channel_name, 0);
			}
		}
		else if(mode[i] == 'k')
		{
			if(mode_sign == 1)
			{
				if(partsCmd.size() > pos)
				{
					server->set_channel_key(channel_name, partsCmd[pos]);
					std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " +k " + partsCmd[pos] + "\r\n";
					brodcast_message(channel_name, server, message);
					pos++;
				}
				else
				{
					client.sendMessage("461 MODE k :Key mode requires a parameter\r\n");
					continue;
				}
			}
			else
			{
				server->remove_channel_key(channel_name);
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " -k\r\n";
				brodcast_message(channel_name, server, message);
			}
		}
		else if(mode[i] == 'l')
		{
			if(mode_sign == 1)
			{
				if(partsCmd.size() > pos)
				{
					if(!isdigit(partsCmd[pos][0]))
					{
						pos++;
						continue;
					}
					double limit = std::strtod(partsCmd[pos].c_str(), NULL);
					if(limit > 9999 || limit < 1)
					{
						pos++;
						continue;
					}
					server->set_channel_limit(channel_name, limit);
					std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " +l " + partsCmd[pos] + "\r\n";
					brodcast_message(channel_name, server, message);
					pos++;
				}
				else
				{
					client.sendMessage("461 MODE l :Limit mode requires a number\r\n");
					continue;
				}
			}
			else
			{
				server->remove_channel_limit(channel_name);
				std ::string message = ":" + client.make_host_name() + " MODE " + channel_name + " -l\r\n";
				brodcast_message(channel_name, server, message);
			}
		}
		else
		{
			client.sendMessage("472 " + std::string(1, mode[i]) + " :is unknown mode char to me\r\n");
			continue;
		}
	}
}
