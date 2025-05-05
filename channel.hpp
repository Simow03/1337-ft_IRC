#ifndef CHANNEL_HPP
#define CHANNEL_HPP



# include <iostream>
#include <string>
#include <vector>
#include "Client.hpp"

// class Client;


class channel
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
        channel(std::string name) : name(name) {
            is_private = 0;
            is_invited = 0;
            topic = "";
            key = "";
        } //-> constructor
        channel(std::string name, std::string key, int i) : name(name), key(key) {
            is_private = i;
            topic = "";
        } //-> constructor
        std::string GetTopic() { return topic; } //-> getter for topic
        std::string GetKey() { return key; } //-> getter for key
        std::string GetName() { return name; } //-> getter for name
        void SetTopic(std::string t) { topic = t; } //-> setter for topic
        int GetMaxUsers() { return max_users; } //-> getter for max_users
        void Setkey(std::string k) { key = k; } //-> setter for key
        int if_invited(Client &c)
        {
            for(size_t i = 0; i < invited.size();i++)
            {
                if(invited[i].getFd() == c.getFd())
                    return 1;
            }
            return 0;
        }
        void add_client(Client &c)
        {
            users.push_back(c);
        }
        int client_exist(Client &c)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i].getFd() == c.getFd())
                    return 1;
            }
            return 0;
        }
        void remove_client(Client &c)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i].getFd() == c.getFd())
                {
                    users.erase(users.begin() + i);
                    return;
                }
            }
        }
        void add_client_as_admin(Client &c)
        {
            admins.push_back(c);
        }
        void SetLimit(int limit)
        {
            max_users = limit;
        }
        Client *GetClient(std::string name)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i].getNickName() == name)
                    return &users[i];
            }
            return NULL;
        }
        int client_is_admin(Client &c)
        {
            for(size_t i = 0; i < admins.size();i++)
            {
                if(admins[i].getFd() == c.getFd())
                    return 1;
            }
            return 0;
        }
        void remove_client_as_admin(Client &c)
        {
            for(size_t i = 0; i < admins.size();i++)
            {
                if(admins[i].getFd() == c.getFd())
                {
                    admins.erase(admins.begin() + i);
                    return;
                }
            }
        }
        int get_size()
        {
            return users.size();
        }
        void add_client_as_invited(Client &c)
        {
            invited.push_back(c);
        }
        void sendMessageToAll(std::string message , Client &client)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i].getFd() != client.getFd())
                    users[i].sendMessage(message);
            }
        }
        std::vector<Client *> get_clients()
        {
            std::vector<Client *> clients;
            for(size_t i = 0; i < users.size();i++)
            {
                clients.push_back(&users[i]);
            }
            return clients;
        }
};




#endif