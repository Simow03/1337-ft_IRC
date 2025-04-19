#include "Client.hpp"

Client::Client(int _fd, std::string &_ip, std::string &_nickname, std::string &_username, bool _hasPass)
	: fd(_fd), ip(_ip), nickname(_nickname), username(_username), hasPass(_hasPass), isOP(false)
{
}

Client::~Client()
{
}

int Client::getFd(void) const
{
	return fd;
}

std::string Client::getNickName(void) const
{
	return nickname;
}

std::string Client::getUserName(void) const
{
	return username;
}

bool Client::getHasPass(void) const
{
	return hasPass;
}

bool Client::getIsOP(void) const
{
	return isOP;
}

void Client::sendMessage(std::string &message) const
{
	send(fd, message.c_str(), message.size(), 0);
}

