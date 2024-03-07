#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

# include <string>
# include <vector>
# include <poll.h>
# include "Client.hpp"
# include <arpa/inet.h>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <map>
# include <sstream>


# include <iomanip> // delete

# define SA struct sockaddr

class Server
{

private:

	const std::string			_port;
	const std::string			_passwd;
	int		 					_servfd;
	std::vector<struct pollfd>	_pollfds;
	std::vector<Client>		_clients;

	// just added
	std::vector<std::string> serverParamiters;
	std::map<std::string, void (Server::*)(Client&)> commandMap;

	void		handleNewConnection();

	void		handleRead(int id);

	void		handleWrite(int id);

	std::string	getCommand(int id);

	// void		handleCommand(int	id);

	void		disconnectClient(int id);

public:

	Server( std::string port, std::string passwd, int fd );

	~Server();

	void		start();

	// functions for debuging
	void	printClients( void );
	void	printpollfds( void );

	// ============================================================ //
	 // parser functions
        bool parseCommandClient(char *buffer, Client& client);
        void handleCommand(std::string& commad);

        // check nick clients
        bool checkAlreadyNick(std::string &nick);

        // command member functions
        void pass(Client& client);
        void user(Client& client);
        void nick(Client& client);

        // channel member functions

        void createChannel(std::string channelName);
        void joinChannel(Client& client, std::string channelName);
        void leaveChannel(Client& client, std::string channelName);
        // void listChannels();

        // send messg
		void 	reply(Client &client, std::string const& reply);

};

#endif //IRCSERVER_HPP
