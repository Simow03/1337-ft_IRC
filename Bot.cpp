#include "Bot.hpp"

Bot::Bot() : name("BOT"), prefix("") {}

Bot::Bot(std::string _name, std::string _prefix) : name(_name), prefix(_prefix) {
    botCommands.push_back("help");
    botCommands.push_back("info");
    botCommands.push_back("ping");
}

Bot::Bot(const Bot &other) : name(other.name), prefix(other.prefix), botCommands(other.botCommands) {}

Bot& Bot::operator=(const Bot &other) {

    if (this != &other) {
        this->name = other.name;
        this->prefix = other.prefix;
        this->botCommands = other.botCommands;
    }

    return *this;
}

Bot::~Bot(){}

bool Bot::processCommand(std::string& command, Client& client, int fd) const{
    if (command.substr(0, prefix.length()) != prefix)
        return false;
    
    std::string trimmedCommand = command.substr(prefix.length());
    
    if (trimmedCommand == "help") {
        execHelpCommand(fd);
        return true;
    }
    else if (trimmedCommand == "info") {
        execInfoCommand(client, fd);
        return true;
    }
    else if (trimmedCommand == "ping") {
        execPingCommand(fd);
        return true;
    }
    else {
        std::string errorMsg = std::string(BOT) + "\n[" + name + "] Unknown command: " + trimmedCommand + "\nUse " + prefix + "help to see available commands.\n\n" + std::string(RESET);
        send(fd, errorMsg.c_str(), errorMsg.size(), 0);
        return false;
    }
}

void Bot::execHelpCommand(int fd) const
{
    std::string helpMssg = std::string(BOT) + "\n[" + name + "] Here are the available commands : \n\n" + std::string(RESET) + std::string(BOLD) + "  -> JOIN :" + std::string(RESET) + " join a channel\n" + std::string(BOLD) + "  -> KICK :" + std::string(RESET) + " eject client from channel\n" + std::string(BOLD) + "  -> INVITE :" + std::string(RESET) + " invite client to a channel\n" + std::string(BOLD) + "  -> TOPIC :" + std::string(RESET) + " change or view the channel topic\n" + std::string(BOLD) + "  -> MODE :" + std::string(RESET) + " change the channel mode: \n"
                               "\t. i: set/remove invite-only channel\n"
                               "\t. t: set/remove the restrictions of the TOPIC command to channel operators\n"
                               "\t. k: set/remove the channel key (password)\n"
                               "\t. o: give/take channel operator privilege\n"
                               "\t. l: set/remove the user limit to channel\n" + std::string(BOLD) + "  -> BOT :" + std::string(RESET) + " call bot to action\n"
                               "\t. !help: ask bot for the available commands and usage\n"
                               "\t. !ping: ping bot\n"
                               "\t. !info: get information about client\n\n";

    send(fd, helpMssg.c_str(), helpMssg.size(), 0);
};

void Bot::execInfoCommand(Client& client, int fd) const
{
    std::string infoMssg = std::string(BOT) + "\n[" + name + "] Here are informations about the client : \n" + std::string(RESET) + std::string(BOLD) + "\nnickname: " + std::string(RESET) + client.getNickName() + std::string(BOLD) + "\nusername: " + std::string(RESET) + client.getUserName() + std::string(BOLD) +  "\nip address: " + std::string(RESET) + client.getIpAddress() + std::string(BOLD) + "\nclient fd: " + std::string(RESET) + std::to_string(client.getFd()) + "\n\n";
    send(fd, infoMssg.c_str(), infoMssg.size(), 0);
}

void Bot::execPingCommand(int fd) const {
    std::string pingMssg  = std::string(BOT) + "\n[" + name + "] PONG - How can I help you ?\n\n" + std::string(RESET);
    send(fd, pingMssg.c_str(), pingMssg.size(), 0);
}

void Bot::execBotCommand(int fd) const {
    std::string pingMssg  = std::string(BOT) + "\n[" + name + "] please use BOT + !<help/info/ping> to access bot service.\n\n" + std::string(RESET);
    send(fd, pingMssg.c_str(), pingMssg.size(), 0);
}

bool Bot::isBotCommand(std::string& command) const{
    return command.substr(0, prefix.length()) == prefix;
}