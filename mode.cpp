#include "parse.hpp"



std::vector<std::string>	Split(std::string recvmessage)
{
	std::vector<std::string> partsCmd;

	std::istringstream stream(recvmessage);

    std::string part;

	while (stream >> part)
    {
        partsCmd.push_back(part);
    }
	return(partsCmd);
}
void parse::execute_mode(std::string arg, Server *server, Client *client)
{
	std ::string channel_name;
	std::string mode;
	int mode_sign = 1;
	int pos = 2;

	if(arg.empty())
	{
		client->sendMessage("ERR_NEEDMOREPARAMS\n");
		return;
	}
	std::vector<std::string> partsCmd = Split(arg);
	if(partsCmd.empty() || partsCmd.size() < 2)
	{
		client->sendMessage("ERR_NEEDMOREPARAMS\n");
		return;
	}
	if(!server->channel_exist(partsCmd[0]))
	{
		client->sendMessage("ERR_NOSUCHCHANNEL\n");
		return;
	}
	channel_name = partsCmd[0];
	mode = partsCmd[1];
	if(server->client_exist(channel_name, client) == 0)
	{
		client->sendMessage("ERR_NOTONCHANNEL\n");
		return;
	}
	if(server->client_isAdmin(channel_name, client) == 0)
	{
		client->sendMessage("ERR_CHANOPRIVSNEEDED\n");
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
					Client *temp = server->GetClientInChannel(client_name);
					if (temp == NULL)
					{
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, temp) == 0)
					{
						pos++;
						continue;
					}
					server->add_client_as_channel_admin(channel_name, temp);
					pos++;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if(partsCmd.size() > pos)
				{
					std::string client_name = partsCmd[pos];
					Client *temp = server->GetClientInChannel(client_name);
					if (temp == NULL)
					{
						pos++;
						continue;
					}
					if(server->clientAdmin(channel_name, temp) != 0)
					{
						pos++;
						continue;
					}
					server->remove_client_as_channel_admin(channel_name, temp);
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
			client->sendMessage("ERR_UNKNOWNMODE\n");
			return;
		}
	}
}
