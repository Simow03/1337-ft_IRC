#include "Server.hpp"

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << "Error : invalid input\n
				Usage : ./ircserv <port> <password>"
				<< std::endl;
		return 1;
	}

	if (!is_valid_port(av[1])) {
		std::cerr << "Error : invalid port number" << std::endl;
		return 1;
	}

	int port = atoi(av[1].c_str);

	std::string password = av[2];

	if (!is_valid_password(password)) {
		std::cerr << "Error : invalid password" << std::endl;
		return 1;
	}

	Server server(port, password);

	return 0;
}