#include "Socket.hpp"

Socket::~Socket()
{
}

Socket::Socket()
{
	// create an end-point communication socket
	if ( (_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* SOCK_NONBLOCK: Using  this  flag  saves extra */
	{																		/* calls to fcntl() to achieve the same result. */
		perror("socket");
		exit(1);
	}
}

int Socket::getfd()
{
	return (_sockfd);
}

void Socket::bindSocket(string port)
{
	struct sockaddr_in	servaddr;

	// fill in an Internet socket address structure with the server's IP address and port number
	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(port.c_str()));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // remove htonl? // INADDR_ANY allows the server to accept a client connection on any interface

	// TODO: Add setsockop() to reuse address

	// bind The server's port to the socket
	if (bind(_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind");
		closeSocket();
		exit(1);
	}
}

void Socket::listenSocket()
{
	if (listen(_sockfd, BACKLOG) == -1)
	{
		perror("listen");
		closeSocket();
		exit(1);
	}
}

void Socket::setSocketNonBlocking()
{
	int	flags;

	if ( (flags = fcntl(_sockfd, F_GETFL)) == -1)
	{
		perror("fcntl");
		closeSocket();
		exit(1);
	}
	flags |= O_NONBLOCK;
	if (fcntl(_sockfd, F_SETFL, flags) == -1)
	{
		perror("fcntl");
		closeSocket();
		exit(1);
	}
}

void Socket::closeSocket(void)
{
	close(_sockfd);
}
