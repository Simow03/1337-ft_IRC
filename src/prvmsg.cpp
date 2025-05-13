#include "../inc/parse.hpp"


void parse::execute_privmsg(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = splitFct(arg);
	if (partsCmd.size() < 2)
	{
		std::string msg = ":IRCserver 461 PRIVMSG :Not enough parameters\r\n";
		client.sendMessage(msg);
		return;
	}
	std::string target = partsCmd[0];
	std::string message = "";
	int isChannel = 0;
	if(server->channel_exist(target))
		isChannel = 1;
	else if (server->client_in_server(target))
		isChannel = 0;
	else
	{
		client.sendMessage(ERR_NOSUCHNICK(target));
		return;
	}

	size_t pos = arg.find(':');
	if (pos != std::string::npos)
		message = arg.substr(pos + 1);
	else
		message = partsCmd[1];
	if(isChannel)
	{
		if (server->client_exist(target, client) == 0)
		{
			client.sendMessage(":IRCServer 442 " + target + " :You're not on that channel\r\n");
			return;
		}
		std :: string msg = ":" + client.info() + " PRIVMSG " + target + " :" + message;
		server->sendMessageToChannel(target, msg + "\r\n", client);
		return;
	}
	Client *temp = server->GetClientInServer(target);
	std::string msg = ":" + client.info() + " PRIVMSG " + target + " :" + message;
	temp->sendMessage(msg + "\r\n");
}
