#include "../inc/Bot.hpp"

Bot::Bot(const std::string &ip, int port, const std::string &password,
         const std::string &nick, const std::string &user)
    : sockfd(-1), nickname(nick), username(user), server_ip(ip),
      server_port(port), server_password(password), authenticated(false)
{
}

void Bot::processMessage(const std::string &message)
{
    size_t prefixEnd = message.find(' ');
    if (prefixEnd == std::string::npos)
        return;

    std::string prefix = message.substr(0, prefixEnd);
    std::string remaining = message.substr(prefixEnd + 1);

    size_t cmdEnd = remaining.find(' ');
    if (cmdEnd == std::string::npos)
        return;

    std::string command = remaining.substr(0, cmdEnd);
    std::string params = remaining.substr(cmdEnd + 1);

    if (isNumeric(command))
    {
        if (command == "001")
        {
            authenticated = true;
            std::cout << GREEN << BOLD << "\n    Successfully authenticated with the server!\n"
                      << RESET << std::endl;
        }
        return;
    }

    if (command == "PRIVMSG")
    {
        size_t targetEnd = params.find(' ');
        if (targetEnd == std::string::npos)
            return;

        std::string target = params.substr(0, targetEnd);
        std::string message = params.substr(targetEnd + 2);

        std::string senderNickname = "";
        if (prefix[0] == ':')
        {
            size_t nickEnd = prefix.find('!');
            if (nickEnd != std::string::npos)
            {
                senderNickname = prefix.substr(1, nickEnd - 1);
            }
        }
        processPrivMsg(senderNickname, target, message);
    }
}

void Bot::processPrivMsg(const std::string &sender, const std::string &target, const std::string &message)
{
    std::cout << "Message from " << BLUE << BOLD << sender << RESET << " to " << BOLD << BLUE << target << RESET << " : " << message << std::endl;

    if (message.length() > 0 && message[0] == '!')
    {
        std::string command = message.substr(1);
        std::string cmdName;
        std::string cmdArgs = "";

        size_t spacePos = command.find(' ');
        if (spacePos != std::string::npos)
        {
            cmdName = command.substr(0, spacePos);
            cmdArgs = command.substr(spacePos + 1);
        }
        else
            cmdName = command;

        if (cmdName == "help")
            sendHelpMessage(sender);
        else if (cmdName == "echo")
            sendEchoMessage(sender, cmdArgs);
        else if (cmdName == "info")
            sendInfoMessage(sender);
        else
            sendPrivMsg(sender, "Invalid command : " + message + ". Please use \'!help\' to see available commands.");
    }
    else
        sendPrivMsg(sender, "Invalid command : " + message + ". Please use \'!help\' to see available commands.");
}

void Bot::sendHelpMessage(const std::string &target)
{
    std::string helpMsg = "Available commands:\r\n"
                          "* !help - List available commands\r\n"
                          "* !echo <message> - Repeat the message\r\n"
                          "* !info - Display basic information about me\r\n"
                          "\r\nIRC Commands:\r\n"
                          "* KICK - Eject a client from the channel\r\n"
                          "* INVITE - Invite a client to a channel\r\n"
                          "* TOPIC - Change or view the channel topic\r\n"
                          "* MODE - Change the channel's mode:\r\n"
                          "  · i: Set/remove Invite-only channel\r\n"
                          "  · t: Set/remove the restrictions of the TOPIC command to channel operators\r\n"
                          "  · k: Set/remove the channel key (password)\r\n"
                          "  · o: Give/take channel operator privilege\r\n"
                          "  · l: Set/remove the user limit to channel\r\n"
                          "* PRIVMSG - Sends private message to a client\r\n"
                          "* QUIT - Quit the server";

    sendPrivMsg(target, helpMsg);
}

void Bot::sendEchoMessage(const std::string &target, const std::string &message)
{
    if (message.empty())
        sendPrivMsg(target, "Echo what? Please provide a message after !echo");
    else
        sendPrivMsg(target, "You said: " + message);
}

void Bot::sendInfoMessage(const std::string &target)
{
    std::string infoMsg = "I am " + nickname + ", an IRC bot.\r\n"
                                               "Username: " +
                          username + "\r\n"
                                     "Server: " +
                          server_ip + ":" + intToString(server_port) + "\r\n"
                                                                       "Status: " +
                          std::string(authenticated ? "Connected" : "Not connected");

    sendPrivMsg(target, infoMsg);
}

void Bot::sendPrivMsg(const std::string &target, const std::string &message)
{
    std::vector<std::string> lines;
    std::stringstream ss(message);
    std::string line;

    while (std::getline(ss, line, '\n'))
        lines.push_back(line);

    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string cmd = "PRIVMSG " + target + " :" + lines[i] + "\r\n";
        send(sockfd, cmd.c_str(), cmd.length(), 0);
    }
}

bool Bot::isNumeric(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return !str.empty();
}

std::string Bot::intToString(int num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

bool Bot::connect()
{
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << RED << "Error creating socket" << RESET << std::endl;
        return false;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    struct hostent *host = gethostbyname(server_ip.c_str());
    if (host == NULL)
    {
        std::cerr << RED << "Invalid address: hostname resolution failed" << RESET << std::endl;
        close(sockfd);
        sockfd = -1;
        return false;
    }
    memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);

    if (::connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        sockfd = -1;
        return false;
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    std::string passCmd = "PASS " + server_password + "\r\n";
    std::string nickCmd = "NICK " + nickname + "\r\n";
    std::string userCmd = "USER " + username + " 0 * :" + username + "\r\n";

    send(sockfd, passCmd.c_str(), passCmd.length(), 0);
    send(sockfd, nickCmd.c_str(), nickCmd.length(), 0);
    send(sockfd, userCmd.c_str(), userCmd.length(), 0);

    return true;
}

void Bot::run()
{
    if (sockfd == -1)
    {
        std::cerr << RED << "\nNot connected to server\n"
                  << RESET << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    std::string message_buffer;

    while (g_bot_running)
    {
        memset(buffer, 0, sizeof(buffer));
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (received > 0)
        {
            buffer[received] = '\0';
            message_buffer += buffer;

            size_t pos;
            while ((pos = message_buffer.find("\r\n")) != std::string::npos)
            {
                std::string message = message_buffer.substr(0, pos);
                message_buffer.erase(0, pos + 2);

                processMessage(message);
            }
        }
        else if (received == 0)
        {
            std::cerr << RED << BOLD << "\nServer disconnected\n"
                      << RESET << std::endl;
            break;
        }
        else
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                std::cerr << RED << BOLD << "\nError receiving data\n"
                          << RESET << std::endl;
                break;
            }
        }

        usleep(100000);
    }

    std::string quitCmd = "QUIT\r\n";
    send(sockfd, quitCmd.c_str(), quitCmd.length(), 0);

    close(sockfd);
    sockfd = -1;
}

Bot::~Bot()
{
    if (sockfd != -1)
        close(sockfd);
}