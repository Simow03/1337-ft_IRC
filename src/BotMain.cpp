#include "../inc/Bot.hpp"

bool g_bot_running = true;

void signalHandler(int signum)
{
    (void)signum;
    g_bot_running = false;
}

int main(int ac, char **av)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    if (ac < 4 || ac > 6)
    {
        std::cerr << RED << BOLD << "\nUsage: " << RESET
                  << av[0] << " <server_ip> <port> <password> [nickname] [username]\n"
                  << std::endl;
        return 1;
    }

    std::string server_ip = av[1];
    int server_port = atoi(av[2]);
    std::string password = av[3];

    std::string nickname = (ac > 4) ? av[4] : "IRCBot";
    std::string username = (ac > 5) ? av[5] : "ircbot";

    std::cout << YELLOW << BOLD << "\n        . . . Starting IRC Bot . . .\n" << RESET << std::endl;
    std::cout << "Server: " << server_ip << ":" << server_port << std::endl;
    std::cout << "Nickname: " << nickname << std::endl;
    std::cout << "Username: " << username << std::endl;

    Bot bot(server_ip, server_port, password, nickname, username);

    if (!bot.connect())
    {
        std::cerr << RED << BOLD << "\n        Failed to connect to server\n"
                  << RESET << std::endl;
        return 1;
    }

    sleep(2);

    bot.run();

    std::cout << YELLOW << BOLD << "\n        IRC Bot has exited!\n"
              << RESET << std::endl;

    return 0;
}
