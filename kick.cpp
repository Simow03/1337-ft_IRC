#include "parse.hpp"


void parse::execute_kick(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage("461 KICK :Not enough parameters\n");
		return;
	}
	std::string channel = partsCmd[0];
	std::string target = partsCmd[1];
	std ::string reason = partsCmd.size() > 2 ? partsCmd[2] : "No reason given";

	if(!server->channel_exist(channel))
	{
		client.sendMessage("403 " + client.getNickName() + " " + channel + " :No such channel\n");
		return;
	}
	if(!server->client_exist(channel, client))
	{
		client.sendMessage("442 " + client.getNickName() + " " + channel + " :You're not on that channel\n");
		return;
	}
	if(!server->client_isAdmin(channel, client))
	{
		client.sendMessage("482 " + client.getNickName() + " " + channel + " :You're not channel operator\n");
		return;
	}
	if(!server->client_exist_by_name(channel, target))
	{
		client.sendMessage("441 " + client.getNickName() + " " + target + " " + channel + " :They aren't on that channel\n");
		return;
	}
	Client *targetClient = server->GetClientInChannel(channel, target);
	if(client.getNickName() == target)
		return;

	std::string prefix = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost";
	std::string msg = prefix + " KICK " + channel + " " + target + " :" + reason + "\n";

	std::vector<Client *> channelClients = server->get_clients_in_channel(channel);
	for (std::vector<Client *>::iterator it = channelClients.begin(); it != channelClients.end(); ++it)
	{
		(*it)->sendMessage(msg);
	}
	server->remove_client_from_channel(channel, *targetClient);
	if(server->clientAdmin(channel, *targetClient) == 1)
	{
		server->remove_client_as_channel_admin(channel, *targetClient);
	}
}