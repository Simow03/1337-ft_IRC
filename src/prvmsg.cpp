#include "../inc/parse.hpp"


void parse::execute_privmsg(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = splitFct(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage(" 461 PRIVMSG :Not enough parameters\r\n");
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
		client.sendMessage(" 401 " + target + " :No such nick/channel\r\n");
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
		std::string msg = ":" + client.getNickName() + "!" + client.getUserName() + " PRIVMSG " + target + " :" + message;
		server->sendMessageToChannel(target, msg + "\r\n", client);
		return;
	}
	Client *temp = server->GetClientInServer(target);
	temp->sendMessage(":" + client.getNickName() + "!" + client.getUserName() + " PRIVMSG " + target + " :" + message + "\r\n");
}

