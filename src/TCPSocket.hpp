#ifndef SOCKETCONNECTION_HPP
# define SOCKETCONNECTION_HPP

# include <unistd.h>
# include <string.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include "error.hpp"
# include <cstdlib>
# include <fcntl.h>


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

#endif //SOCKETCONNECTION_HPP
