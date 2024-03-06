#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <poll.h>
# include "IRCClient.hpp"
# include <arpa/inet.h>

# define SA struct sockaddr

class IRCServer
{
private:

	const std::string			_port;
	const std::string			_passwd;
	int		 					_servfd;
	std::vector<struct pollfd>	pollfds;
	std::vector<IRCClient>			clients;

	void	handleNewConnection();

public:
	IRCServer( std::string port, std::string passwd, int fd );
	~IRCServer();

	void		start();
};

void IRCServer::handleNewConnection()
{
	struct sockaddr_in	cliaddr;
	char				buffer[512];
	int					connfd;
	socklen_t			len;
	struct pollfd		pfd;
	int					flags;

	len = sizeof(cliaddr);
	if ( (connfd = accept(_servfd, (SA *) (&cliaddr), &len)) == -1)	// use c++ cast
		errQuit("accept", 1);
	if (!inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer)))
		errQuit("inet_ntop", 1);
	// std::cout << "ip: " << buffer << " Port: " << ntohs(cliaddr.sin_port) << std::endl;

	// Add the new client socket to clients and pollfds
	// IRCClient cli(buffer, ntohs(cliaddr.sin_port), connfd);
	clients.push_back(IRCClient(buffer, ntohs(cliaddr.sin_port), connfd));

	pfd.fd = connfd;
	pfd.events = POLLIN | POLLHUP | POLLOUT;
	pollfds.push_back(pfd);

	// set socket to Non-blocking
	if ( (flags = fcntl(connfd, F_GETFL)) == -1
		|| fcntl(connfd, F_SETFL, flags | O_NONBLOCK) == -1)
		errQuit("fcntl", 1);
}

IRCServer::IRCServer(std::string port, std::string passwd, int fd)
	: _port(port), _passwd(passwd), _servfd(fd)
{
	// First entry in the pollfds array is used for the listening socket
	struct pollfd servPoll;
	servPoll.fd = _servfd;
	servPoll.events = POLLRDNORM;	// specify events of interest on fd
	pollfds.push_back(servPoll);
}

IRCServer::~IRCServer()
{
}

void	IRCServer::start()
{
	int					connfd;
	struct sockaddr_in	cliaddr;
	socklen_t			len = sizeof(cliaddr);
	int					nready;

	for ( ; ; ) {

		if (( nready = poll(&pollfds[0], pollfds.size(), 0)) == -1)
			errQuit("poll", 1);

		if (nready == 0)
			continue;

		if (pollfds[0].revents & POLLIN) { // listening socket is readable

			// Accept new connection
			// set socket to Non-blocking
			// Add the new client socket to clients and pollfds
			handleNewConnection();
		}

		if (!--nready) continue;

		for (int k = 1; k < pollfds.size(); k++) { // check for returned events on other sockets

			if (pollfds[k].revents & (POLLHUP | POLLIN | POLLERR)) {

				// read data from client
				// if read() returns 0 the client is disconnected
				// When a client disconnects, close its socket.
				// Remove the client socket from the pollfd array and clients.
				nready--;
			}
			// if (pollfds[k].revents & POLLOUT) {
			//	// Socket is ready for writing
			// }
			if (!nready) break;
		}

	}
}

#endif
