#include "parse.hpp"


int parse::cmd_lenght(std::string str)
{
    int i = 0;
    while(str[i] && str[i] != ' ')
        i++;
    return i;
}
void parse::execute_join(std::string arg, Server *server, Client *client)
{
    // std::cout << client->getFd() << std::endl;
    std::string ch;
    std::string key;
    int i = 0;
    while(arg[i] && arg[i] != ' ')
        i++;
    ch = arg.substr(0, i);
    if(arg[i])
        key = arg.substr(i + 1);
    if(arg == "0")
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
    if(channels.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    int j = 0;
    while (j < channels.size())
    {
        if (channels[j][0] != '#' && channels[j][0] != '&')
        {
            // client->sendMessage("ERR_NOSUCHCHANNEL\n");
            std::cout << "ERR_NOSUCHCHANNEL" << std::endl;
        }
        else
        {
            if(server->channel_exist(channels[j]))
            {
                if (server->client_exist(channels[j], client))
                {

                    std::cout << "Client already in channel" << std::endl;
                    // client->sendMessage("ERR_USERONCHANNEL\n");
                }
                else
                {
                   std :: cout << "add client to channel" << std::endl;
                   if(server->channel_need_inv(channels[i]) && server->is_invitd(channels[j],client))
                   {
                        std::cout << "Client not invited" << std::endl;
                   }
                   else if(server->channel_need_inv(channels[i]) && !server->is_invitd(channels[j],client))
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
                                 // client->sendMessage("ERR_BADCHANNELKEY\n");
                                 std::cout << "ERR_BADCHANNELKEY" << std::endl;
                             }
                             else
                             {
                                 server->add_client_to_channel(channels[j], client);
                                 std::cout << "Client added to channel" << std::endl;
                             }
                        }
                        else
                        {
                             server->add_client_to_channel(channels[j], client);
                        }
                   }
                }
            }
            else
            {
                if (!keys.empty() &&  j < keys.size())
                {
                    server->add_channel(channels[j], keys[j], 1);
                }
                else
                {
                    server->add_channel(channels[j], "", 0);
                }
                server->add_client_to_channel(channels[j], client);
                std::cout << "Channel created" << channels[j] << std::endl;
            }
        }
        j++;
    }

}

void parse::execute_cmd(std::string cmd, std::string arg, Server *server, Client *client)
{
    if(cmd == "JOIN")
    {
        execute_join(arg, server, client);
    }
}

parse::parse(char* buff, Server* server, Client* client)
{
    int len = cmd_lenght(buff);
    cmd = std::string(buff, len);
    arg = std::string(buff + len + 1);
    execute_cmd(cmd, arg, server, client);
}