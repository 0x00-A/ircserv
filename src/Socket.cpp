#include "Socket.hpp"

Socket::~Socket()
{
}

Socket::Socket() : _sockfd(-1)
{
}

int Socket::getfd()
{
	return (_sockfd);
}

void Socket::listenSocket(const string port)
{
	struct addrinfo hints, *res, *ressave;
	int	n;
	const int	enable = 1;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

	if ( (n = getaddrinfo(NULL, port.c_str(), &hints, &res)))	// return 0 on success
	{
		cerr << "getaddrinfo error: " + string(gai_strerror(n)) << endl;
		exit(1);
	}
	ressave = res;
	do
	{
		/* code */
		this->_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (_sockfd < 0)
			continue;
		setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
		if (bind(_sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		closeSocket();
		
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
	{
		perror("error");
		exit(1);
	}
	if (listen(_sockfd, BACKLOG) == -1)
	{
		perror("listen");
		closeSocket();
		exit(1);
	}
	freeaddrinfo(ressave);
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
