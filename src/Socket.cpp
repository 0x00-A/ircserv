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
	const int	enable = 1;

	// fill in an Internet socket address structure with the server's IP address and port number
	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(port.c_str()));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // remove htonl? // INADDR_ANY allows the server to accept a client connection on any interface

	// TODO: Add setsockop() to reuse address
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
	{
		perror("setsockopt");
	}

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

// #include "Socket.hpp"

// Socket::~Socket()
// {
// }

// Socket::Socket() : _sockfd(-1)
// {
// }

// int Socket::getfd()
// {
// 	return (_sockfd);
// }

// void Socket::listenSocket(const string port)
// {
// 	struct addrinfo hints, *res, *ressave;
// 	int	n;
// 	const int	enable = 1;

// 	bzero(&hints, sizeof(hints));
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_protocol = IPPROTO_TCP;
// 	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

// 	if ( (n = getaddrinfo(NULL, port.c_str(), &hints, &res)))	// return 0 on success
// 	{
// 		cerr << "getaddrinfo error: " + string(gai_strerror(n)) << endl;
// 		exit(1);
// 	}
// 	ressave = res;
// 	do
// 	{
// 		/* code */
// 		this->_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
// 		if (_sockfd < 0)
// 			continue;
// 		setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
// 		if (bind(_sockfd, res->ai_addr, res->ai_addrlen) == 0)
// 			break;
// 		closeSocket();
		
// 	} while ( (res = res->ai_next) != NULL);

// 	if (res == NULL)
// 	{
// 		perror("error");
// 		exit(1);
// 	}
// 	if (listen(_sockfd, BACKLOG) == -1)
// 	{
// 		perror("listen");
// 		closeSocket();
// 		exit(1);
// 	}
// 	freeaddrinfo(ressave);
// }

// void Socket::setSocketNonBlocking()
// {
// 	int	flags;

// 	if ( (flags = fcntl(_sockfd, F_GETFL)) == -1)
// 	{
// 		perror("fcntl");
// 		closeSocket();
// 		exit(1);
// 	}
// 	flags |= O_NONBLOCK;
// 	if (fcntl(_sockfd, F_SETFL, flags) == -1)
// 	{
// 		perror("fcntl");
// 		closeSocket();
// 		exit(1);
// 	}
// }

// void Socket::closeSocket(void)
// {
// 	close(_sockfd);
// }