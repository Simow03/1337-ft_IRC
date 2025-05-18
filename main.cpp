#include "inc/Server.hpp"

bool g_running = true;

void signalHandler(int signum)
{
	(void)signum;
	g_running = false;
}

bool is_valid_port(std::string portStr)
{

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

bool password_invalid(std::string password)
{
	return (password.empty() || password.find(' ') != std::string::npos || password.find_first_not_of(" \t\n\r") == std::string::npos);
}

int main(int ac, char **av)
{

	if (ac != 3)
	{
		std::cerr << RED << BOLD << UNDERLINE << "\nError :"
				  << RESET << " invalid input\n"
				  << RED << BOLD << UNDERLINE << "\nUsage :"
				  << RESET << "./ircserv <port> <password>\n"
				  << std::endl;
		return 1;
	}

	if (!is_valid_port(av[1]))
	{
		std::cerr << RED << BOLD << UNDERLINE << "\nError :"
				  << RESET << " invalid port\n"
				  << RED << BOLD << UNDERLINE << "\nUsage :"
				  << RESET << " Port should be between 1024 - 49151\n"
				  << std::endl;
		return 1;
	}

	std::stringstream ss;
	int port;

	ss << av[1];
	ss >> port;

	std::string password = av[2];

	if (password_invalid(password))
	{
		std::cerr << RED << BOLD << UNDERLINE << "\nError :"
				  << RESET << " invalid password\n"
				  << RED << BOLD << UNDERLINE << "\nUsage :"
				  << RESET << " Password should not be empty or contains a space.\n"
				  << std::endl;
		return 1;
	}

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);

	Server server(port, password);

	server.runServer();

	std::cout << YELLOW << BOLD << "\n\t      Server has exited !      \n"
			  << RESET << std::endl;

	return 0;
}