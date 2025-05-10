#include "../inc/parse.hpp"

void parse::execute_topic(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = splitFct(arg);
	if (partsCmd.size() < 1)
	{
		client.sendMessage("461 TOPIC :Not enough parameters\r\n");
		return;
	}
	std::string channel_name = partsCmd[0];
	if(!server->channel_exist(channel_name))
	{
		client.sendMessage("403 " + channel_name + " :No such channel\r\n");
		return;
	}
	if(server->client_exist(channel_name, client) == 0)
	{
		client.sendMessage("442 " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	if(server->is_topic_restricted(channel_name) == 1 && server->client_isAdmin(channel_name, client) == 0)
	{
		client.sendMessage("482 " + channel_name + " :You're not channel operator\r\n");
		return;
	}

	if(partsCmd.size() == 1)
	{
		std ::string topic = server->GetTopic(channel_name);
		if (topic == "")
		{
			client.sendMessage("332 " + client.getNickName() + " " + channel_name + " :No topic is set\r\n");
			return;
		}
		client.sendMessage("332 " + client.getNickName() + " " + channel_name + " :" + topic + "\r\n");
	}
	else
	{
		std::string topic = "";
		size_t pos = arg.find(':');
		if (pos != std::string::npos)
			topic = arg.substr(pos + 1);
		else
			topic = partsCmd[1];
		if (topic == "")
		{
			server->set_topic(channel_name, "");
			client.sendMessage("332 " + client.getNickName() + " " + channel_name + " :" + "\r\n");
			return;
		}
		server->set_topic(channel_name, topic);
		client.sendMessage("332 " + client.getNickName() + " " + channel_name + " :" + topic + "\r\n");
		return;
	}
}