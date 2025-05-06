#include "../inc/parse.hpp"

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
        client.sendMessage("421 " + client.getNickName() + " " + cmd + " :Unknown command\n");
    }
}



parse::parse(std::string buff, Server* server, Client &client)
{
    std::vector<std::string> partsCmd = Split(buff);
    cmd = partsCmd[0];
    if(partsCmd.size() > 1)
    {
        arg = buff.substr(cmd.size() + 1);
    }
    else
        arg = "";
    execute_cmd(cmd, arg, server, client);
}
