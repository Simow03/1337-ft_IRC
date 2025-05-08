#include "../inc/parse.hpp"


void send_msg_to_invited(Client &client, std::string channel_name, std::string client_name, Server *server)
{
	Client *inviter = server->GetClientInServer(client_name);
	if (inviter == NULL)
	{
		client.sendMessage("401 " + client.getNickName() + " " + client_name + " :No such nick/channel\r\n");
		return;
	}
	std::string prefix = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIpAddress();
	client.sendMessage("341 " + client.getNickName() + " " + client_name + " " + channel_name + "\r\n");

	inviter->sendMessage(prefix + " INVITE " + client_name + " :" + channel_name + "\r\n");

	std::cout << prefix + " INVITE " + client_name + " :" + channel_name << std::endl;
}

void parse::execute_invite(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = splitFct(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage("461 INVITE :Not enough parameters\r\n");
		return;
	}
	std::string channel_name = partsCmd[1];
	std::string client_name = partsCmd[0];
	Client *temp = server->GetClientInServer(client_name);
	if(server->client_in_server(client_name) == 0)
	{
		client.sendMessage("401 " + client.getNickName() + " " + client_name + " :No such nick/channel\r\n");
		return;
	}
	if (server->channel_exist(channel_name) == 0)
	{
		client.sendMessage("403 " + client.getNickName() + " " + channel_name + " :No such channel\r\n");
		return;
	}
	if(server->client_exist(channel_name, client) == 0)
	{
		client.sendMessage("442 " + client.getNickName() + " " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	if (server->client_exist_by_name(channel_name, client_name) == 1)
	{
		client.sendMessage("443 " + client.getNickName() + " " + client_name + " " + channel_name + " :is already on channel\r\n");
		return;
	}
	if (server->channel_need_inv(channel_name) == 1)
	{
		if(server->client_isAdmin(channel_name, client) == 0)
		{
			client.sendMessage("482 " + client.getNickName() + " " + channel_name + " :You're not channel operator\r\n");
			return;
		}
		server->add_client_as_invited(channel_name, *temp);
		send_msg_to_invited(client, channel_name, client_name, server);

	}
	else
	{
		server->add_client_as_invited(channel_name, *temp);
		send_msg_to_invited(client, channel_name, client_name, server);
	}
}