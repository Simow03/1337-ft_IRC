#include "parse.hpp"


void parse::execute_kick(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage("461 KICK :Not enough parameters\r\n");
		return;
	}
	std::string channel = partsCmd[0];
	std::string target = partsCmd[1];
	std ::string reason ;
	if (partsCmd.size() > 2)
	{
		for (size_t i = 2; i < partsCmd.size(); ++i)
		{
			reason += partsCmd[i];
			if (i != partsCmd.size() - 1)
				reason += " ";
		}
	}
	else
	{
		reason = "No reason given";
	}

	if(!server->channel_exist(channel))
	{
		client.sendMessage("403 " + client.getNickName() + " " + channel + " :No such channel\r\n");
		return;
	}
	if(!server->client_exist(channel, client))
	{
		client.sendMessage("442 " + client.getNickName() + " " + channel + " :You're not on that channel\r\n");
		return;
	}
	if(!server->client_isAdmin(channel, client))
	{
		client.sendMessage("482 " + client.getNickName() + " " + channel + " :You're not channel operator\r\n");
		return;
	}
	if(!server->client_exist_by_name(channel, target))
	{
		client.sendMessage("441 " + client.getNickName() + " " + target + " " + channel + " :They aren't on that channel\r\n");
		return;
	}
	Client *targetClient = server->GetClientInChannel(channel, target);
	if(client.getNickName() == target)
		return;

	std::string prefix = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost";
	std::string msg = prefix + " KICK " + channel + " " + target + " :" + reason + "\r\n";

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

	std::vector<Client *> members = server->get_clients_in_channel(channel);
    std::string client_list = "353 " + client.getNickName() + " = " + channel + " :";
    for (size_t i = 0; i < members.size(); ++i)
    {
        client_list += members[i]->getNickName() + " ";
    }
    client_list += "\r\n";
    client.sendMessage(client_list);
    std::string end_list = "366 " + client.getNickName() + " " + channel + " :End of NAMES list\r\n";
    client.sendMessage(end_list);
}