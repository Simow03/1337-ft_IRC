#include "parse.hpp"


int parse::cmd_lenght(std::string str)
{
    int i = 0;
    while(str[i] && str[i] != ' ')
        i++;
    return i;
}

void parse::execute_cmd(std::string cmd, std::string arg, Server *server, Client &client)
{
    if (cmd == "JOIN")
    {
        execute_join(arg, server, client);
    }
    else if (cmd == "MODE")
    {
        execute_mode(arg, server, client);
    }
}



parse::parse(char* buff, Server* server, Client &client)
{
    int len = cmd_lenght(buff);
    cmd = std::string(buff, len);
    arg = std::string(buff + len + 1);
    execute_cmd(cmd, arg, server, client);
}