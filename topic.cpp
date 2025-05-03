#include "parse.hpp"

void parse::execute_topic(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 1)
	{
		client.sendMessage("461 TOPIC :Not enough parameters\n");
		return;
	}
	std::string channel_name = partsCmd[0];
	if(!server->channel_exist(channel_name))
	{
		client.sendMessage("403 " + channel_name + " :No such channel\n");
		return;
	}
	if(server->client_exist(channel_name, client) == 0)
	{
		client.sendMessage("442 " + channel_name + " :You're not on that channel\n");
		return;
	}

	if(server->is_topic_restricted(channel_name) == 0)
	{
		if(partsCmd.size() == 1)
		{
			std ::string topic = server->GetTopic(channel_name);
			client.sendMessage("TOPIC " + channel_name + " :" + topic + "\n");
		}
		else
		{
			std::string check = partsCmd[1];
			std::string topic ;
			if(check == ":")
			{
				if(partsCmd.size() == 2)
				{
					server->set_topic(channel_name, "");
					client.sendMessage("TOPIC " + channel_name + " :" + "\n");
					return;
				}
				for(size_t i = 2; i < partsCmd.size(); i++)
				{
					topic += partsCmd[i];
					if(i != partsCmd.size() - 1)
						topic += " ";
				}
				server->set_topic(channel_name, topic);
				client.sendMessage("TOPIC " + channel_name + " :" + topic + "\n");
				return;
			}
			server->set_topic(channel_name, check);
			client.sendMessage("TOPIC " + channel_name + " :" + check + "\n");
			return;
		}
	}
	else
	{
		if(server->client_isAdmin(channel_name, client) == 0)
		{
			client.sendMessage("482 " + channel_name + " :You're not channel operator\n");
			return;
		}
		if(partsCmd.size() == 1)
		{
			std ::string topic = server->GetTopic(channel_name);
			client.sendMessage("TOPIC " + channel_name + " :" + topic + "\n");
		}
		else
		{
			std::string check = partsCmd[1];
			std::string topic ;
			if(check == ":")
			{
				if(partsCmd.size() == 2)
				{
					server->set_topic(channel_name, "");
					client.sendMessage("TOPIC " + channel_name + " :" + "\n");
					return;
				}
				for(size_t i = 2; i < partsCmd.size(); i++)
				{
					topic += partsCmd[i];
					if(i != partsCmd.size() - 1)
						topic += " ";
				}
				server->set_topic(channel_name, topic);
				client.sendMessage("TOPIC " + channel_name + " :" + topic + "\n");
				return;
			}
			server->set_topic(channel_name, check);
			client.sendMessage("TOPIC " + channel_name + " :" + check + "\n");
			return;
		}
	}

}