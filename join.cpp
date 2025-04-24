#include "parse.hpp"



void parse::execute_join(std::string arg, Server *server, Client &client)
{
    // std::cout << client->getFd() << std::endl;
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
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    size_t j = 0;
    while (j < channels.size())
    {
        if (channels[j][0] != '#' && channels[j][0] != '&')
        {
            client.sendMessage("ERR_NOSUCHCHANNEL\n");
            std::cout << "ERR_NOSUCHCHANNEL" << std::endl;
        }
        else
        {
            if (server->channel_exist(channels[j]))
            {
				if (server->client_exist(channels[j], client))
				{

					std::cout << "Client already in channel" << std::endl;
					client.sendMessage("ERR_USERONCHANNEL\n");
				}
				else
                {
                   std :: cout << "Attempting to add client to channel" << std::endl;
                   if (server->channel_need_inv(channels[j]) && !server->is_invitd(channels[j],client))
                   {
                        std::cout << "Client not invited" << std::endl;
                   }
                   else if (server->channel_need_inv(channels[j]) && server->is_invitd(channels[j],client))
                   {
                        std::cout << "Client is invited" << std::endl;
                        server->add_client_to_channel(channels[j], client);
                   }
                   else
                   {
                        if (server->channel_has_key(channels[j]))
                        {
                             std::string k = server->get_channel_key(channels[j]);
                             if (j >= keys.size() || keys[j] != k)
                             {
                                 client.sendMessage("ERR_BADCHANNELKEY\n");
                                 std::cout << "ERR_BADCHANNELKEY" << std::endl;
                             }
                             else
                             {
                                if(server->limit(channels[j]) == 1)
                                {
                                    client.sendMessage("ERR_CHANNELISFULL\n");
                                    std::cout << "ERR_CHANNELISFULL" << std::endl;
                                    return;
                                }
                                 server->add_client_to_channel(channels[j], client);
                                 std::cout << "Client added to channel" << std::endl;
                             }
                        }
                        else
                        {
                            if(server->limit(channels[j]) == 1 )
                            {
                                client.sendMessage("ERR_CHANNELISFULL\n");
                                std::cout << "ERR_CHANNELISFULL" << std::endl;
                                return;
                            }
                             server->add_client_to_channel(channels[j], client);
                        }
                   }
                }
            }
            else
            {
                server->add_channel(channels[j]);
                server->add_client_as_channel_admin(channels[j], client);
                server->add_client_to_channel(channels[j], client);
                std::cout << "Channel created " << channels[j] << "!"<< std::endl;
            }
        }
        j++;
    }
}