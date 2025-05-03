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
# include "repl.hpp"
class Server;
// #include "Server.hpp"

class channel;

enum ClientState {
	CONNECTING,
	CONNECTED,
	AUTHENTICATED
};

class Client
{
private:
	int fd;
	std::string ip;
	std::string nickname;
	std::string username;

	bool hasPass;
	bool isAuthenticated;
	bool isOP;
	ClientState state;

public:
	Client(int _fd, std::string &_ip, std::string &_nickname, std::string &_username, bool _hasPass, bool _isAuthenticated);
	~Client();

	int getFd(void) const;
	std::string getNickName(void) const;
	std::string getUserName(void) const;
	std::string getIpAddress(void) const;
	bool getIsAuthenticated(void) const;
	bool getHasPass(void) const;
	ClientState getState(void) const;
	bool getIsOP(void) const;
	void setNickName(std::string& nickname);
	void setUserName(std::string& username);
	void setIsAuthenticated(bool var);
	void setHasPass(bool var);
	void setIsOP(bool var);
	void setState(ClientState var);

	void sendMessage(std::string message) const;
};

#endif