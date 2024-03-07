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

class TCPSocket
{

private:

	int		_sockfd;

	TCPSocket();

public:

	~TCPSocket();

	TCPSocket( const char* port );

	int		getfd( void );

	void	close( void );
	
};

#endif //TCPSOCKET_HPP
