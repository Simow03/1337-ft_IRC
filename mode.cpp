#include "parse.hpp"

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
	std::vector<std::string> partsCmd = Split(arg);
	if(partsCmd.empty() || partsCmd.size() < 2)
	{
		client.sendMessage("461 MODE :Not enough parameters\r\n");
		return;
	}
	if(!server->channel_exist(partsCmd[0]))
	{
		client.sendMessage("403 " + channel_name + " :No such channel\r\n");
		return;
	}
	channel_name = partsCmd[0];
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
				server->set_channel_invite(channel_name, 1);
			else
				server->set_channel_invite(channel_name, 0);
		}
		else if(mode[i] == 'o')
		{
			if (mode_sign == 1)
			{
				if(partsCmd.size() > pos)
				{
					std::string client_name = partsCmd[pos];
					Client *temp = server->GetClientInChannel(channel_name, client_name);
					if (temp == NULL)
					{
						client.sendMessage("401 " + client_name + " :No such nick/channel\r\n");
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, *temp) == 0)
					{
						pos++;
						continue;
					}
					server->add_client_as_channel_admin(channel_name, *temp);
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
					Client *temp = server->GetClientInChannel(channel_name, client_name);
					if (temp == NULL)
					{
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, *temp) != 0)
					{
						pos++;
						continue;
					}
					server->remove_client_as_channel_admin(channel_name, *temp);
					pos++;
				}
			}
		}
		else if(mode[i] == 't')
		{
			if(mode_sign == 1)
				server->set_channel_topic(channel_name, 1);
			else
				server->set_channel_topic(channel_name, 0);
		}
		else if(mode[i] == 'k')
		{
			if(mode_sign == 1)
			{
				if(partsCmd.size() > pos)
				{
					server->set_channel_key(channel_name, partsCmd[pos]);
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
			}
		}
		else
		{
			client.sendMessage("472 " + std::string(1, mode[i]) + " :is unknown mode char to me\r\n");
			return;
		}
		std::string prefix = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost";
		std::string finalMsg = prefix + " MODE " + channel_name + " " + mode;
		if (!mode.empty())
		{
			finalMsg += " " + mode;
		}
		finalMsg += "\r\n";

		std::vector<Client *> members = server->get_clients_in_channel(channel_name);
		for (size_t i = 0; i < members.size(); ++i)
		{
			members[i]->sendMessage(finalMsg);
		}
	}
}
