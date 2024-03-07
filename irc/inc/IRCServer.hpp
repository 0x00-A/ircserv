#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

# include <string>
# include <vector>
# include <poll.h>
# include "IRCClient.hpp"
# include <arpa/inet.h>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>

# include <iomanip> // delete

# define SA struct sockaddr

class IRCServer
{

private:

	const std::string			_port;
	const std::string			_passwd;
	int		 					_servfd;
	std::vector<struct pollfd>	_pollfds;
	std::vector<IRCClient>		_clients;

	void		handleNewConnection();

	void		handleRead(int id);

	void		handleWrite(int id);

	std::string	getCommand(int id);

	// void		handleCommand(int	id);

	void		disconnectClient(int id);

public:

	IRCServer( std::string port, std::string passwd, int fd );

	~IRCServer();

	void		start();

	// functions for debuging
	void	printClients( void );
	void	printpollfds( void );

};

#endif //IRCSERVER_HPP
