#ifndef PARSE_HPP
#define PARSE_HPP


#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "Server.hpp"
#include "Client.hpp"
#include "channel.hpp"
// #include <iomanip>



class Server;
class Client;
class channel; //-> forward declaration of channel class

class parse
{
    private:
        std::string cmd;
        std::string arg;
        std::vector<std::string> channels;
        std::vector<std::string> keys;
        void execute_cmd(std::string cmd, std::string arg, Server *server, Client &client);
        int cmd_lenght(std::string str);
        void execute_join(std::string arg, Server *server, Client &client);
        void execute_mode(std::string arg, Server *server, Client &client);
        void execute_invite(std::string arg, Server *server, Client &client);

    public:
        parse(char *buff,Server *server,Client &client);



};
std::vector<std::string>	Split(std::string recvmessage);




#endif