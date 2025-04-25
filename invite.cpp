#include "parse.hpp"


void send_msg_to_invited(Client &client, std::string channel_name, std::string client_name, Server *server)
{
	std::string msg = "INVITE " + client.getNickName() + " " + channel_name + "\n";
	client.sendMessage(msg);
	std::cout << "INVITE " + client.getNickName() + " " + channel_name + "\n";
	Client *temp = server->GetClientInServer(client_name);
	if (temp == NULL)
	{
		client.sendMessage("ERR_NOSUCHNICK\n");
		return;
	}
	std::string msg2 = "INVITE " + client.getNickName() + " " + channel_name + "\n";
	temp->sendMessage(msg2);
}

void parse::execute_invite(std::string arg, Server *server, Client &client)
{
	std::vector<std::string> partsCmd = Split(arg);
	if (partsCmd.size() < 2)
	{
		client.sendMessage("ERR_NEEDMOREPARAMS\n");
		return;
	}
	std::string channel_name = partsCmd[1];
	std::string client_name = partsCmd[0];
	Client *temp = server->GetClientInServer(client_name);
	if(server->client_in_server(client_name) == 0)
	{
		client.sendMessage("ERR_NOSUCHNICK\n");
		return;
	}
	if (server->channel_exist(channel_name) == 0)
	{
		client.sendMessage("ERR_NOSUCHCHANNEL\n");
		return;
	}
	if(server->client_exist(channel_name, client) == 0)
	{
		client.sendMessage("ERR_NOTONCHANNEL\n");
		return;
	}
	if (server->client_exist_by_name(channel_name, client_name) == 1)
	{
		client.sendMessage("ERR_USERONCHANNEL\n");
		return;
	}
	if (server->channel_need_inv(channel_name) == 1)
	{
		if(server->client_isAdmin(channel_name, client) == 0)
		{
			client.sendMessage("ERR_CHANOPRIVSNEEDED\n");
			return;
		}
		std ::cout << "client name: " << client_name << std::endl;
		server->add_client_as_invited(channel_name, *temp);
		send_msg_to_invited(client, channel_name, client_name, server);

	}
	else
	{
		server->add_client_as_invited(channel_name, *temp);
		send_msg_to_invited(client, channel_name, client_name, server);
	}
}