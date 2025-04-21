#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <string>
#include <vector>

class Client;


class channel
{
    private:
        std::string name;
        std::string topic;
        std::string key;
        std::vector<Client*> users;
        std::vector<Client*> admins;
        std::vector<Client*> invited;
        int max_users;
    public:
        int is_private;
        int is_invited;
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
        int if_invited(Client *c)
        {
            for(size_t i = 0; i < invited.size();i++)
            {
                if(invited[i] == c)
                    return 1;
            }
            return 0;
        }
        void add_client(Client *c)
        {
            users.push_back(c);
        }
        int client_exist(Client *c)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i] == c)
                    return 1;
            }
            return 0;
        }
        void remove_client(Client *c)
        {
            for(size_t i = 0; i < users.size();i++)
            {
                if(users[i] == c)
                {
                    users.erase(users.begin() + i);
                    return;
                }
            }
        }
};




#endif