#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

# include <unistd.h>
# include <string.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <cstdlib>
# include <fcntl.h>
# include <cstdio>


# define BACKLOG 50

class Socket
{

private:

	int		_sockfd;

	Socket();

public:

	~Socket();

	Socket( const char* port );

	int		getfd( void );

	void	closeSocket( void );
	
};

#endif //TCPSOCKET_HPP
