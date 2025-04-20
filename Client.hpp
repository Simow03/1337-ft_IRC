#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

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