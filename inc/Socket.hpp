#ifndef SOCKET_HPP
#define SOCKET_HPP

# include <netdb.h>
# include "ircserv.hpp"
# include <sys/socket.h>
# include <netinet/in.h>

# define BACKLOG 50

class Socket
{

private:

	int		_sockfd;

	Socket();

public:

	Socket( const string port );

	~Socket();

	int		getfd( void );

	void	bindSocket(string port);

	void	listenSocket();

	void	setSocketNonBlocking();

	void	closeSocket( void );
	
};

#endif //SOCKET_HPP
