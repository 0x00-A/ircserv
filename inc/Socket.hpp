#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netdb.h>
#include "ircserv.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SA struct sockaddr

#define BACKLOG 50

class Socket
{
	private:
		int _sockfd;
		
	public:
		Socket();

		~Socket();

		int		getfd( void );

		void	createSocket( void );

		void	bindSocket( string port );

		void	listenSocket( void );

		void	setSocketNonBlocking( void );

		void	closeSocket( void );
};

#endif // SOCKET_HPP
