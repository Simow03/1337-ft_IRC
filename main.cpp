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
		std::cerr << RED <<  BOLD << UNDERLINE << "\nError :"
				<< RESET <<" invalid input\n"
				<< RED <<  BOLD << UNDERLINE << "\nUsage :"
				<< RESET << "./ircserv <port> <password>\n" << std::endl;
		return 1;
	}
 
	if (!is_valid_port(av[1])) {
		std::cerr << RED <<  BOLD << UNDERLINE << "\nError :"
				<< RESET <<" invalid port\n"
				<< RED <<  BOLD << UNDERLINE << "\nUsage :"
				<< RESET << " Port should be between 1024 - 49151\n" << std::endl;
		return 1;
	}

	std::stringstream ss;
	int port;

	ss << av[1];
	ss >> port;

	std::string password = av[2];

	if (password.empty()) {
		std::cerr << RED <<  BOLD << UNDERLINE << "\nError :"
				<< RESET <<" invalid password\n"
				<< RED <<  BOLD << UNDERLINE << "\nUsage :"
				<< RESET << " Password should not be empty.\n" << std::endl;
		return 1;
	}

	Server server(port, password);

	server.runServer();

	return 0;
}