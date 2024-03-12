#ifndef SOCKET_HPP
#define SOCKET_HPP

# include <netdb.h>
# include "ircserv.hpp"
# include <sys/socket.h>
# include <netinet/in.h>

# include <sys/types.h>

# define BACKLOG 50

class Socket
{

private:

	int		_sockfd;

public:

	Socket();

	// Socket( const string port );

	~Socket();

	int		getfd( void );

	void	listenSocket( const string port );

	void	setSocketNonBlocking();

	void	closeSocket( void );
	
};

#endif //SOCKET_HPP
