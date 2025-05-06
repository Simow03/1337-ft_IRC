#ifndef BOT_HPP
# define BOT_HPP

# include "Server.hpp"
# include "Client.hpp"

# define BOT "\033[1;36m"
# define BOLD "\033[1m"
# define RESET "\033[0m"

class Client;

class Bot
{
private:
    std::string name;
    std::string prefix;
    std::vector<std::string> botCommands;
public:
    Bot();
    Bot(std::string name, std::string prefix);
    Bot(const Bot &other);
    Bot& operator=(const Bot &other);
    ~Bot();
    void execBotCommand(int fd, const std::string& target) const;
    void execHelpCommand(int fd, const std::string& target) const;
    void execPingCommand(int fd, const std::string& target) const;
    void execInfoCommand(Client& client, int fd, const std::string& target) const;
    bool processCommand(std::string& command, Client& client, int fd) const;
    void sendIrcMessage(int fd, const std::string& target, const std::string& message) const;
    void sendMultilineMessage(int fd, const std::string& target, const std::string& message) const;

    bool isBotCommand(std::string &command) const;
};

#endif