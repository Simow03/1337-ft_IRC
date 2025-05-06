#include "../inc/Server.hpp"

void Server::add_channel(std::string name)
{
    Channel c(name);
    channels.push_back(c);
}
int Server::channel_exist(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
            return 1;
    }
    return 0;
}
int Server::client_exist(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].client_exist(client))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
void Server::add_client_to_channel(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].add_client(client);
            return;
        }
    }
}
int Server::channel_has_key(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].is_private == 1)
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
std ::string Server::get_channel_key(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            return channels[i].GetKey();
        }
    }
    return "";
}
void Server::remove_client_from_channels(Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].client_exist(client))
        {
            channels[i].remove_client(client);
        }
    }
}
int Server::channel_need_inv(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].is_invited == 1)
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
int Server::is_invitd(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].if_invited(client))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
void Server::add_client_as_channel_admin(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].add_client_as_admin(client);
            return;
        }
    }
}
void Server::set_channel_invite(std::string channel_name, int i)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].is_invited = i;
            return;
        }
    }
}
void Server::set_channel_topic(std::string channel_name, int i)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].topic_restricted = i;
            return;
        }
    }
}
int Server::client_isAdmin(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].client_is_admin(client))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
void Server::set_channel_limit(std::string channel_name, int limit)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].SetLimit(limit);
            channels[j].is_limited = 1;
            return;
        }
    }
}
void Server::remove_channel_limit(std::string channel_name)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].is_limited = 0;
            return;
        }
    }
}
void Server::set_channel_key(std::string channel_name, std::string key)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].is_private = 1;
            channels[j].Setkey(key);
            return;
        }
    }
}
void Server::remove_channel_key(std::string channel_name)
{
    for (size_t j = 0; j < channels.size(); j++)
    {
        if (channels[j].GetName() == channel_name)
        {
            channels[j].is_private = 0;
            return;
        }
    }
}
Client* Server::GetClientInChannel(std::string channel_name, std::string client_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            return channels[i].GetClient(client_name);
        }
    }
    return NULL;
}
int Server::clientAdmin(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            if (channels[i].client_is_admin(client))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}
void Server::remove_client_as_channel_admin(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].remove_client_as_admin(client);
            return;
        }
    }
}
int Server::limit(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            int limit = channels[i].GetMaxUsers();
            int size = channels[i].get_size();
            if (channels[i].is_limited == 1)
            {
                if (size >= limit)
                    return 1;
                return 0;
            }
            return 0;
        }
    }
    return 0;
}
int Server::client_exist_by_name(std::string channel_name, std::string client_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            Client *temp = channels[i].GetClient(client_name);
            if (temp == NULL)
                return 0;
            else
                return 1;
        }
    }
    return 0;
}
int Server::client_in_server(std::string client_name)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].getNickName() == client_name)
            return 1;
    }
    return 0;
}
void Server::add_client_as_invited(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].add_client_as_invited(client);
            return;
        }
    }
}
Client* Server::GetClientInServer(std::string client_name)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].getNickName() == client_name)
            return &clients[i];
    }
    return NULL;
}
void Server::remove_client_from_channel(std::string channel_name, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].remove_client(client);
            return;
        }
    }
}
int Server::is_topic_restricted(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            return channels[i].topic_restricted;
        }
    }
    return 0;
}
std::string Server::GetTopic(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            return channels[i].GetTopic();
        }
    }
    return "";
}
void Server::set_topic(std::string channel_name, std::string topic)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].SetTopic(topic);
            return;
        }
    }
}
void Server::sendMessageToChannel(std::string channel_name, std::string message, Client &client)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            channels[i].sendMessageToAll(message, client);
            return;
        }
    }
}

std::vector<Client *> Server::get_clients_in_channel(std::string channel_name)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].GetName() == channel_name)
        {
            return channels[i].get_clients();
        }
    }
    return std::vector<Client *>();
}
std::vector<std::string> Server::get_channels(Client c)
{
    std::vector<std::string> channels_names;
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (client_exist(channels[i].GetName(), c))
            channels_names.push_back(channels[i].GetName());
    }
    return channels_names;
}