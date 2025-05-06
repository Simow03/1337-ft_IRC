#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
private:
    std::string name;
    std::string topic;
    std::string key;
    std::vector<Client> users;
    std::vector<Client> admins;
    std::vector<Client> invited;
    int max_users;

public:
    int is_limited;
    int is_private;
    int is_invited;
    int topic_restricted;
    Channel(std::string name);
    Channel(std::string name, std::string key, int i);
    std::string GetTopic();
    std::string GetKey();
    std::string GetName();
    void SetTopic(std::string t);
    int GetMaxUsers();
    void Setkey(std::string k);
    int if_invited(Client &c);
    void add_client(Client &c);
    int client_exist(Client &c);
    void remove_client(Client &c);
    void add_client_as_admin(Client &c);
    void SetLimit(int limit);
    Client *GetClient(std::string name);
    int client_is_admin(Client &c);
    void remove_client_as_admin(Client &c);
    int get_size();
    void add_client_as_invited(Client &c);
    void sendMessageToAll(std::string message, Client &client);
    std::vector<Client *> get_clients();
};

#endif