#include "parse.hpp"



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
    if (arg == "0")
    {
        server->remove_client_from_channels(client);
        return;
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
        }
        else
        {
            if (server->channel_exist(channels[j]))
            {
				if (server->client_exist(channels[j], client))
				{
					client.sendMessage("443 " + client.getNickName() + " " + channels[j] + " :is already on channel\r\n");
				}
				else
                {
                   if (server->channel_need_inv(channels[j]) && !server->is_invitd(channels[j],client))
                   {
                        client.sendMessage("473 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+i)\r\n");
                   }
                   else if (server->channel_need_inv(channels[j]) && server->is_invitd(channels[j],client))
                   {
                        server->add_client_to_channel(channels[j], client);
                        std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                for (size_t i = 0; i < members.size(); ++i)
			                members[i]->sendMessage(RPL_JOINMSG(client.getHostname(), client.getIpAddress(), channels[j]));
                   }
                   else
                   {
                        if (server->channel_has_key(channels[j]))
                        {
                             std::string k = server->get_channel_key(channels[j]);
                             if (j >= keys.size() || keys[j] != k)
                             {
                                client.sendMessage("475 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+k)\r\n");
                             }
                             else
                             {
                                if(server->limit(channels[j]) == 1)
                                {
                                    client.sendMessage("471 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+l)\r\n");
                                    continue;
                                }
                                server->add_client_to_channel(channels[j], client);
                                // client.sendMessage("JOIN " + channels[j] + "\n");
                                 std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                        for (size_t i = 0; i < members.size(); ++i)
			                        members[i]->sendMessage(RPL_JOINMSG(client.getHostname(), client.getIpAddress(), channels[j]));
                             }
                        }
                        else
                        {
                            if(server->limit(channels[j]) == 1 )
                            {
                                client.sendMessage("471 " + client.getNickName() + " " + channels[j] + " :Cannot join channel (+l)\r\n");
                                continue;
                            }
                             server->add_client_to_channel(channels[j], client);
                            //  client.sendMessage("JOIN " + channels[j] + "\n");
                                 std::vector<Client *> members = server->get_clients_in_channel(channels[j]);
		                        for (size_t i = 0; i < members.size(); ++i)
			                        members[i]->sendMessage(RPL_JOINMSG(client.getHostname(), client.getIpAddress(), channels[j]));
                        }
                   }
                }
            }
            else
            {
        // std::vector<client> channelClients = channel->getClients();
        // for (std::vector<client>::iterator it = channelClients.begin(); it != channelClients.end(); ++it) {
        //     sendReponse(RPL_JOINMSG(currentClient.getHostname(), currentClient.getipadd(), channelName), it->getfd());
        // }
                server->add_channel(channels[j]);
                server->add_client_as_channel_admin(channels[j], client);
                server->add_client_to_channel(channels[j], client);
                // #define RPL_JOINMSG(hostname, ipaddress, channelname)	(":" + hostname + "@" + ipaddress + " JOIN " + channelname + POSTFIX)

                // client.sendMessage(":@"  + client.getNickName() + "!" + client.getUserName() + "@localhost JOIN " + channels[j] + "\r\n");
                client.sendMessage(RPL_JOINMSG(client.getHostname(), client.getIpAddress(), channels[j]));
                // execute_mode(channels[j] + " -i", server, client);
            }
        }
        j++;
    }
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (server->is_topic_restricted(channels[i]))
        {
            std::string topic = server->GetTopic(channels[i]);
            client.sendMessage("332 " + client.getNickName() + " " + channels[i] + " :" + topic + "\r\n");
        }
    }
    // purpose of this is to send the list of clients in the channel
    std::vector<Client *> members = server->get_clients_in_channel(channels[0]);
    std::string client_list ;

    for (size_t i = 0; i < members.size(); ++i)
    {
        if(server->client_isAdmin(channels[0], *members[i]))
            client_list += "@" + members[i]->getNickName() + " ";
        else
            client_list += members[i]->getNickName() + " ";
    }

    // client_list += "\r\n";
    // client.sendMessage(client_list);
    // std::string end_list = ":ircserv 366 " + client.getNickName() + " " + channels[0] + " :End of NAMES list\r\n";
    // client.sendMessage(end_list);
    // std::cout << "=>" << client_list << std::endl;
    // Names: :ircserv 353 flick = #hahahaha :@flick

    // Names: :ircserv 353 flick = #hahahaha :@flick
    //        :ircserv 353 flick = #hahahaha :@flick
        client.sendMessage(RPL_NAMREPLY(client.getNickName() , channels[0], client_list));
        client.sendMessage(RPL_ENDOFNAMES(client.getNickName() , channels[0]));
}
