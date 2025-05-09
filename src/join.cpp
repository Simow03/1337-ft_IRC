#include "../inc/parse.hpp"



void parse::execute_join(std::string arg, Server *server, Client &client)
{
    std::string ch;
    std::string key;
    int i = 0;
    while(arg[i] && arg[i] != ' ')
        i++;
    ch = arg.substr(0, i);
    if (arg[i])
	{
		while(arg[i] && arg[i] == ' ')
			i++;
        key = arg.substr(i);
	}
    std::stringstream ss(ch);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        channels.push_back(token);
    }
    std::stringstream ss2(key);
    while (std::getline(ss2, token, ','))
    {
        keys.push_back(token);
    }
    if (channels.empty())
    {
        client.sendMessage("461 JOIN :Not enough parameters\r\n");
        return;
    }
    size_t j = 0;
    while (j < channels.size())
    {
        if (channels[j][0] != '#' && channels[j][0] != '&')
        {
            client.sendMessage("403 " + client.getNickName() + " " + channels[j] + " :No such channel\r\n");
            j++;
            continue;
        }
        else
        {
            if (server->channel_exist(channels[j]))
            {
				if (server->client_exist(channels[j], client))
				{
					client.sendMessage("443 " + client.getNickName() + " " + channels[j] + " :is already on channel\r\n");
                    j++;
                    continue;
				}
				else
                {
                   if (server->channel_need_inv(channels[j]) && !server->is_invitd(channels[j],client))
                   {
                        client.sendMessage("473 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+i)\r\n");
                        j++;
                        continue;
                   }
                   else if (server->channel_need_inv(channels[j]) && server->is_invitd(channels[j],client))
                   {
                        server->add_client_to_channel(channels[j], client);
                        std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                for (size_t i = 0; i < members.size(); ++i)
			                members[i]->sendMessage(RPL_JOINMSG(client.make_host_name(), client.getIpAddress(), channels[j]));
                   }
                   else
                   {
                        if (server->channel_has_key(channels[j]))
                        {
                             std::string k = server->get_channel_key(channels[j]);
                             if (j >= keys.size() || keys[j] != k)
                             {
                                client.sendMessage("475 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+k)\r\n");
                                j++;
                                continue;
                             }
                             else
                             {
                                if(server->limit(channels[j]) == 1)
                                {
                                    client.sendMessage("471 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+l)\r\n");
                                    j++;
                                    continue;
                                }
                                server->add_client_to_channel(channels[j], client);
                                 std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                        for (size_t i = 0; i < members.size(); ++i)
			                        members[i]->sendMessage(RPL_JOINMSG(client.make_host_name(), client.getIpAddress(), channels[j]));
                             }
                        }
                        else
                        {
                            if(server->limit(channels[j]) == 1 )
                            {
                                client.sendMessage("471 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+l)\r\n");
                                j++;
                                continue;
                            }
                             server->add_client_to_channel(channels[j], client);
                                 std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                        for (size_t i = 0; i < members.size(); ++i)
			                        members[i]->sendMessage(RPL_JOINMSG(client.make_host_name(), client.getIpAddress(), channels[j]));
                        }
                   }
                }
            }
            else
            {
                server->add_channel(channels[j]);
                server->add_client_as_channel_admin(channels[j], client);
                server->add_client_to_channel(channels[j], client);
                client.sendMessage(RPL_JOINMSG(client.make_host_name(), client.getIpAddress(), channels[j]));
            }
        }
        if (server->is_topic_restricted(channels[j]))
        {
            std::string topic = server->GetTopic(channels[j]);
            client.sendMessage("332 " + client.getNickName() + " " + channels[j] + " :" + topic + "\r\n");
        }
        j++;
    }
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::vector<Client *> members = server->get_clients_in_channel(channels[i]);
        std::string client_list = "";

        for (size_t x = 0; x < members.size(); x++)
        {
            if(server->client_isAdmin(channels[i], *members[x]))
                client_list += "@" + members[x]->getNickName() + " ";
            else
                client_list += members[x]->getNickName() + " ";

        }
        client.sendMessage(RPL_NAMREPLY(client.getNickName() , channels[i], client_list));
        client.sendMessage(RPL_ENDOFNAMES(client.getNickName() , channels[i]));
    }
}
