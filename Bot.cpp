#include "Bot.hpp"

#define IRC_BOLD "\x02"
#define IRC_COLOR "\x03"
#define IRC_RESET "\x0F"
#define IRC_INVERSE "\x16"
#define IRC_UNDERLINE "\x1F"
#define IRC_COLOR_BLACK "01"
#define IRC_COLOR_BLUE "02"
#define IRC_COLOR_GREEN "03"
#define IRC_COLOR_RED "04"
#define IRC_COLOR_BROWN "05"
#define IRC_COLOR_PURPLE "06"
#define IRC_COLOR_ORANGE "07"
#define IRC_COLOR_YELLOW "08"
#define IRC_COLOR_LIGHT_GREEN "09"
#define IRC_COLOR_TEAL "10"
#define IRC_COLOR_LIGHT_CYAN "11"
#define IRC_COLOR_LIGHT_BLUE "12"
#define IRC_COLOR_PINK "13"
#define IRC_COLOR_GREY "14"
#define IRC_COLOR_LIGHT_GREY "15"
#define IRC_COLOR_WHITE "16"

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

void Bot::sendIrcMessage(int fd, const std::string& target, const std::string& message) const {
    std::string formattedMsg = ":" + name + "!bot@server PRIVMSG " + target + " :" + message + "\r\n";
    send(fd, formattedMsg.c_str(), formattedMsg.size(), 0);
}

void Bot::sendMultilineMessage(int fd, const std::string& target, const std::string& message) const {
    std::istringstream stream(message);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            sendIrcMessage(fd, target, line);
        }
    }
}

bool Bot::processCommand(std::string& command, Client& client, int fd) const {
    if (command.substr(0, prefix.length()) != prefix)
        return false;
    
    std::string trimmedCommand = command.substr(prefix.length());
    std::string target = client.getNickName();
    
    if (trimmedCommand == "help") {
        execHelpCommand(fd, target);
        return true;
    }
    else if (trimmedCommand == "info") {
        execInfoCommand(client, fd, target);
        return true;
    }
    else if (trimmedCommand == "ping") {
        execPingCommand(fd, target);
        return true;
    }
    else {
        std::string errorMsg = IRC_COLOR + std::string(IRC_COLOR_RED) + "[" + name + "] Unknown command: " + trimmedCommand + ". Use " + prefix + "help to see available commands." + IRC_RESET;
        sendIrcMessage(fd, target, errorMsg);
        return false;
    }
}

void Bot::execHelpCommand(int fd, const std::string& target) const {
    sendIrcMessage(fd, target, IRC_COLOR + std::string(IRC_COLOR_BLUE) + "[" + name + "] Here are the available commands:" + IRC_RESET);
    
    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> JOIN :") + IRC_RESET + " join a channel");
    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> KICK :") + IRC_RESET + " eject client from channel");
    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> INVITE :") + IRC_RESET + " invite client to a channel");
    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> TOPIC :") + IRC_RESET + " change or view the channel topic");

    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> MODE :") + IRC_RESET + " change the channel mode:");
    sendIrcMessage(fd, target, "    . i: set/remove invite-only channel");
    sendIrcMessage(fd, target, "    . t: set/remove the restrictions of the TOPIC command to channel operators");
    sendIrcMessage(fd, target, "    . k: set/remove the channel key (password)");
    sendIrcMessage(fd, target, "    . o: give/take channel operator privilege");
    sendIrcMessage(fd, target, "    . l: set/remove the user limit to channel");

    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> BOT :") + IRC_RESET + " call bot to action");
    sendIrcMessage(fd, target, "    . !help: ask bot for the available commands and usage");
    sendIrcMessage(fd, target, "    . !ping: ping bot");
    sendIrcMessage(fd, target, "    . !info: get information about client");

    sendIrcMessage(fd, target, IRC_BOLD + std::string("  -> QUIT :") + IRC_RESET + " disconnect from the server");
}

void Bot::execInfoCommand(Client& client, int fd, const std::string& target) const {
    sendIrcMessage(fd, target, IRC_COLOR + std::string(IRC_COLOR_BLUE) + "[" + name + "] Here are informations about the client:" + IRC_RESET);
    sendIrcMessage(fd, target, IRC_BOLD + std::string("nickname: ") + IRC_RESET + client.getNickName());
    sendIrcMessage(fd, target, IRC_BOLD + std::string("username: ") + IRC_RESET + client.getUserName());
    sendIrcMessage(fd, target, IRC_BOLD + std::string("ip address: ") + IRC_RESET + client.getIpAddress());
    sendIrcMessage(fd, target, IRC_BOLD + std::string("client fd: ") + IRC_RESET + std::to_string(client.getFd()));
}

void Bot::execPingCommand(int fd, const std::string& target) const {
    sendIrcMessage(fd, target, IRC_COLOR + std::string(IRC_COLOR_GREEN) + "[" + name + "] PONG - How can I help you?" + IRC_RESET);
}

void Bot::execBotCommand(int fd, const std::string& target) const {
    sendIrcMessage(fd, target, IRC_COLOR + std::string(IRC_COLOR_BLUE) + "[" + name + "] please use BOT + !<help/info/ping> to access bot service." + IRC_RESET);
}

bool Bot::isBotCommand(std::string& command) const {
    return command.substr(0, prefix.length()) == prefix;
}

Bot::~Bot(){}
