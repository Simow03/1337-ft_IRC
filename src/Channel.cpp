#include "../inc/Channel.hpp"

Channel::Channel(std::string name) : name(name)
{
    is_private = 0;
    is_invited = 0;
    topic = "";
    key = "";
}

Channel::Channel(std::string name, std::string key, int i) : name(name), key(key)
{
    is_private = i;
    topic = "";
}

std::string Channel::GetTopic() { return topic; }
std::string Channel::GetKey() { return key; }
std::string Channel::GetName() { return name; }
void Channel::SetTopic(std::string t) { topic = t; }
int Channel::GetMaxUsers() { return max_users; }
void Channel::Setkey(std::string k) { key = k; }
int Channel::if_invited(Client &c)
{
    for (size_t i = 0; i < invited.size(); i++)
    {
        if (invited[i].getFd() == c.getFd())
            return 1;
    }
    return 0;
}
void Channel::add_client(Client &c)
{
    users.push_back(c);
}
int Channel::client_exist(Client &c)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].getFd() == c.getFd())
            return 1;
    }
    return 0;
}
void Channel::remove_client(Client &c)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].getFd() == c.getFd())
        {
            users.erase(users.begin() + i);
            return;
        }
    }
}
void Channel::add_client_as_admin(Client &c)
{
    admins.push_back(c);
}
void Channel::SetLimit(int limit)
{
    max_users = limit;
}
Client *Channel::GetClient(std::string name)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].getNickName() == name)
            return &users[i];
    }
    return NULL;
}
int Channel::client_is_admin(Client &c)
{
    for (size_t i = 0; i < admins.size(); i++)
    {
        if (admins[i].getFd() == c.getFd())
            return 1;
    }
    return 0;
}
void Channel::remove_client_as_admin(Client &c)
{
    for (size_t i = 0; i < admins.size(); i++)
    {
        if (admins[i].getFd() == c.getFd())
        {
            admins.erase(admins.begin() + i);
            return;
        }
    }
}
int Channel::get_size()
{
    return users.size();
}
void Channel::add_client_as_invited(Client &c)
{
    invited.push_back(c);
}
void Channel::sendMessageToAll(std::string message, Client &client)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].getFd() != client.getFd())
            users[i].sendMessage(message);
    }
}
std::vector<Client *> Channel::get_clients()
{
    std::vector<Client *> clients;
    for (size_t i = 0; i < users.size(); i++)
    {
        clients.push_back(&users[i]);
    }
    return clients;
}