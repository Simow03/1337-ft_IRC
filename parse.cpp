#include "parse.hpp"

std::vector<std::string>	Split(std::string recvmessage)
{
	std::vector<std::string> partsCmd;

	std::istringstream stream(recvmessage);

    std::string part;

	while (stream >> part)
    {
        partsCmd.push_back(part);
    }
	return(partsCmd);
}
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
    else if(cmd == "INVITE")
    {
        execute_invite(arg, server, client);
    }
    else if(cmd == "KICK")
    {
        execute_kick(arg, server, client);
    }
    else if(cmd == "TOPIC")
    {
        execute_topic(arg, server, client);
    }
    else if(cmd =="PRIVMSG")
    {
        execute_privmsg(arg, server, client);
    }
    else if(cmd == "")
        return;
    else
    {
        client.sendMessage("ERR_UNKNOWNCOMMAND\n");
    }
}



parse::parse(char* buff, Server* server, Client &client)
{
    int len = cmd_lenght(buff);
    cmd = std::string(buff, len);
    arg = std::string(buff + len + 1);
    execute_cmd(cmd, arg, server, client);
}