#include "Server.hpp"


extern bool g_bot_running;

class Bot
{
private:
    int sockfd;
    std::string nickname;
    std::string username;
    std::string server_ip;
    int server_port;
    std::string server_password;
    bool authenticated;

public:
    Bot(const std::string &ip, int port, const std::string &password, const std::string &nick, const std::string &user);
    ~Bot();

    bool connect(void);
    void run(void);

    void processMessage(const std::string &message);
    void processPrivMsg(const std::string &sender, const std::string &target, const std::string &message);
    void sendHelpMessage(const std::string &target);
    void sendEchoMessage(const std::string &target, const std::string &message);
    void sendInfoMessage(const std::string &target);
    void sendPrivMsg(const std::string &target, const std::string &message);
    bool isNumeric(const std::string &str);
    std::string intToString(int num);
};