#ifndef SERVER_HPP
#define SERVER_HPP

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
# include <map>
# include <set>
# include <cstring>
# include <cerrno>
# include <signal.h>
# include "Channel.hpp"
# include "parse.hpp"
# include "Client.hpp"
# include "repl.hpp"

#define BACKLOG 1024
#define BUFFER_SIZE 1024
extern bool g_running;

#define RED "\033[31m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define BLUE "\033[34m"

class Client;
class Channel;

class Server
{
private:
	int port;
	std::string password;
	struct sockaddr_in sa;
	struct sockaddr_in ca;
	socklen_t ca_len;
	struct pollfd pollfds;
	std::vector<struct pollfd> clientfds;
	std::vector<Channel> channels;
	std::set<int> newlyConnectedClients;
	int sockfd;
	int clientfd;
	int pollStatus;
	std::map<int, std::string> appendToBuffer;

public:
	std::vector<Client> clients;
	Server(int port, std::string &password);
	~Server();

	void runServer(void);
	void acceptNewConnection();
	bool receiveClientData(size_t i);
	void disconnectClient(size_t i);
	void processCommand(std::string &command, int fd, size_t i);
	void sendIRCReply(int fd, const std::string &prefix, const std::string &command, const std::vector<std::string> &params);
	void sendNumericReply(int fd, const std::string &numeric, const std::string &nickname, const std::vector<std::string> &params);
	void sendWelcomeMessage(int fd, std::string nickname);
	void shutDownServer(void);

	void add_channel(std::string name);
	int channel_exist(std::string channel_name);
	int client_exist(std::string channel_name, Client &client);
	void add_client_to_channel(std::string channel_name, Client &client);
	int channel_has_key(std::string channel_name);
	std ::string get_channel_key(std::string channel_name);
	void remove_client_from_channels(Client &client);
	int channel_need_inv(std::string channel_name);
	int is_invitd(std::string channel_name, Client &client);
	void add_client_as_channel_admin(std::string channel_name, Client &client);
	void set_channel_invite(std::string channel_name, int i);
	void set_channel_topic(std::string channel_name, int i);
	int client_isAdmin(std::string channel_name, Client &client);
	void set_channel_limit(std::string channel_name, int limit);
	void remove_channel_limit(std::string channel_name);
	void set_channel_key(std::string channel_name, std::string key);
	void remove_channel_key(std::string channel_name);
	int	GetClientInChannel(std::string channel_name, std::string client_name);
	int clientAdmin(std::string channel_name, Client &client);
	void remove_client_as_channel_admin(std::string channel_name, Client &client);
	int limit(std::string channel_name);
	int client_exist_by_name(std::string channel_name, std::string client_name);
	int client_in_server(std::string client_name);
	void add_client_as_invited(std::string channel_name, Client &client);
	Client *GetClientInServer(std::string client_name);
	void remove_client_from_channel(std::string channel_name, Client &client);
	int is_topic_restricted(std::string channel_name);
	std::string GetTopic(std::string channel_name);
	void set_topic(std::string channel_name, std::string topic);
	void sendMessageToChannel(std::string channel_name, std::string message , Client &client);
	std::vector<Client *> get_clients_in_channel(std::string channel_name);
	std::vector<std::string> get_channels(Client c);
	bool isValidNickname(std::string &nickname) const;
};

#endif