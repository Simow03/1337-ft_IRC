#include "parse.hpp"


void parse::execute_privmsg(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage(" 461 PRIVMSG :Not enough parameters\n");
		return;
	}
	std::string target = partsCmd[0];
	std::string message = "";
	int isChannel = 0;
	if(server->channel_exist(target))
	{
		isChannel = 1;
	}
	else if (server->client_in_server(target))
	{
		isChannel = 0;
	}
	else
	{
		client.sendMessage(" 401 " + target + " :No such nick/channel\n");
		return;
	}
	if(partsCmd[1][0] != ':')
		message += partsCmd[1];
	else
	{
		for(size_t i = 1; i < partsCmd.size(); i++)
		{
			message += partsCmd[i];
			if (i != partsCmd.size() - 1)
				message += " ";
		}
	}
	if(isChannel)
	{
		server->sendMessageToChannel(target, message);
		return;
	}
	Client *temp = server->GetClientInServer(target);
	temp->sendMessage("PRIVMSG " + client.getNickName() + " :" + message + "\n");
}