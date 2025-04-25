#include "parse.hpp"


void parse::execute_kick(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage("ERR_NEEDMOREPARAMS\n");
		return;
	}
	std::string channel = partsCmd[0];
	std::string target = partsCmd[1];
	std ::string reason = partsCmd.size() > 2 ? partsCmd[2] : "No reason given";

	if(!server->channel_exist(channel))
	{
		client.sendMessage("ERR_NOSUCHCHANNEL\n");
		return;
	}
	if(!server->client_exist(channel, client))
	{
		client.sendMessage("USENOTINCHANNEL\n");
		return;
	}
	if(!server->client_isAdmin(channel, client))
	{
		client.sendMessage("ERR_CHANOPRIVSNEEDED\n");
		return;
	}
	if(!server->client_exist_by_name(channel, target))
	{
		client.sendMessage("ERR_NOSUCHNICK\n");
		return;
	}
	Client *targetClient = server->GetClientInChannel(channel, target);
	if(client.getNickName() == target)
		return;
	server->remove_client_from_channel(channel, *targetClient);
	if(server->clientAdmin(channel, *targetClient) == 1)
	{
		server->remove_client_as_channel_admin(channel, *targetClient);
	}
	// msg
	std::string msg = ":" + client.getNickName() + " KICK " + channel + " " + target + " :" + reason + "\n";
}