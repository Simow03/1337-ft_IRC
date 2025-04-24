#ifndef CLIENT_HPP
#define CLIENT_HPP
# include <iostream>

# include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <unistd.h>
# include <vector>
# include <cstring>
# include <cerrno>
# include <signal.h>
class Server;
// #include "Server.hpp"

class channel;

class Client
{
private:
	int fd;
	std::string ip;
	std::string nickname;
	std::string username;

	bool hasPass;
	bool isOP;

public:
	Client(int _fd, std::string &_ip, std::string &_nickname, std::string &_username, bool _hasPass);
	~Client();

	int getFd(void) const;
	std::string getNickName(void) const;
	std::string getUserName(void) const;
	bool getHasPass(void) const;
	bool getIsOP(void) const;

	void sendMessage(std::string message) const;
};




#endif