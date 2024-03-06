#include "TCPSocket.hpp"

TCPSocket::TCPSocket()
{
}

TCPSocket::~TCPSocket()
{
}

TCPSocket::TCPSocket(const char* port)
{
	struct sockaddr_in servaddr;

	// create an end-point communication socket
	if ( (_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) /* SOCK_NONBLOCK: Using  this  flag  saves extra */
	{																		/* calls to fcntl() to achieve the same result. */
		std::perror("socket");
		exit(1);
	}

	// fill in an Internet socket address structure with the server's IP address and port number
	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(port));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // remove htonl? // INADDR_ANY allows the server to accept a client connection on any interface

	// TODO: Add setsockop() to reuse address

	// bind The server's port to the socket
	if (bind(_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		std::perror("bind");
		close();
		exit(1);
	}

	// convert to a listening socket so that it may receive incoming connections from clients
	if (listen(_sockfd, BACKLOG) == -1)
	{
		std::perror("listen");
		close();
		exit(1);
	}

	// Now we can accept connections using accept()

}

int TCPSocket::getfd()
{
	return (_sockfd);
}

void TCPSocket::close(void)
{
	::close(_sockfd);
}
