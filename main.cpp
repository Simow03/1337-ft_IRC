#include "Server.hpp"

bool is_valid_port(std::string portStr) {

	if (portStr.empty())
		return false;

	for (size_t i = 0; i < portStr.length(); i++)
		if (!isdigit(portStr[i]))
			return false;
	
	std::stringstream ss;
	int port;

	ss << portStr;
	ss >> port;

	if (port < 1024 || port > 49151)
		return false;

	return true;
}

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << "Error : invalid input\nUsage : ./ircserv <port> <password>" << std::endl;
		return 1;
	}
 
	if (!is_valid_port(av[1])) {
		std::cerr << "Error : invalid port number" << std::endl;
		return 1;
	}

	std::stringstream ss;
	int port;

	ss << av[1];
	ss >> port;

	std::string password = av[2];

	if (password.empty()) {
		std::cerr << "Error : invalid password" << std::endl;
		return 1;
	}

	std::cout << "port input : " << port << std::endl;
	std::cout << "password input : " << password << std::endl;

	Server server(port, password);

	return 0;
}