#include "Client.hpp"


Client::Client(int _fd, std::string &_ip, std::string &_nickname, std::string &_username, bool _hasPass, bool _isAuthenticated)
	: fd(_fd), ip(_ip), nickname(_nickname), username(_username), hasPass(_hasPass), isAuthenticated(_isAuthenticated), isOP(false), state(CONNECTING)
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

void Client::setNickName(std::string& _nickname)
{
	nickname = _nickname;
}

void Client::setUserName(std::string& _username)
{
	username = _username;
}

void Client::setIsAuthenticated(bool var)
{
	isAuthenticated = var;
}

std::string Client::getIpAddress(void) const {
	return ip;
}

bool Client::getHasPass(void) const
{
	return hasPass;
}

bool Client::getIsOP(void) const
{
	return isOP;
}

void Client::setHasPass(bool var)
{
	hasPass = var;
}

void Client::setIsOP(bool var)
{
	isOP = var;
}

bool Client::getIsAuthenticated(void) const
{
	return isAuthenticated;
}

void Client::sendMessage(std::string message) const
{
	send(fd, message.c_str(), message.size(), 0);
}

ClientState Client::getState(void) const {
	return state;
};

void Client::setState(ClientState var) {
	state = var;
};