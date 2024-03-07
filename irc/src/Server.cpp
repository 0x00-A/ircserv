#include "Server.hpp"

void Server::handleNewConnection()
{
	struct sockaddr_in	cliaddr;
	char				buffer[512];
	int					connfd;
	socklen_t			len;
	int					flags;

	len = sizeof(cliaddr);
	if ( (connfd = accept(_servfd, (SA *) (&cliaddr), &len)) == -1)	// use c++ cast
	{
		// close sockets
		std::perror("accept");
		exit(1);
	}

	// get ip address
	if (!inet_ntop(AF_INET, &cliaddr.sin_addr, buffer, sizeof(buffer)))
	{
		std::perror("inet_ntop error");
		return;
	}

	// Add the new client socket to _clients and _pollfds
	_clients.push_back(Client(buffer, ntohs(cliaddr.sin_port), connfd));
	_pollfds.push_back((struct pollfd){.fd = connfd, .events = (POLLIN | POLLHUP)});

	// set socket to be Non-blocking
	if ( (flags = fcntl(connfd, F_GETFL)) == -1
		|| fcntl(connfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		// close sockets
		std::perror("fcntl");
		exit(1);
	}

	std::cout << "client " << connfd << " connected" << std::endl;
}

void Server::disconnectClient(int id)
{
	std::cout << "client " << _pollfds[id+1].fd << " disconnected" << std::endl;

	std::vector<Client>::iterator		cli_it;
	std::vector<struct pollfd>::iterator	poll_it;

	cli_it = _clients.begin() + id;

	cli_it->close();
	_clients.erase(cli_it);

	poll_it = _pollfds.begin() + id + 1;
	_pollfds.erase(poll_it);
}

void Server::handleRead(int id)
{
	char		readbuf[512];
	ssize_t		bytesread;

	bytesread = read(_clients[id].getSockfd(), readbuf, sizeof(readbuf));
	if (bytesread <= 0)
	{
		std::perror("read");
		return;
	} else {

		readbuf[bytesread] = '\0';

		_clients[id].rdBuf() += std::string(readbuf);
		
		std::string reply = "srv recved msg: " + std::string(readbuf);


		_clients[id].sdBuf() = reply;
		handleWrite(id);
		std::cout << id << " rdbuf: " << _clients[id].rdBuf() << std::endl;
	}

}

std::string Server::getCommand(int id)
{
	std::string& rdBuf = _clients[id].rdBuf();
	size_t	pos;

	if (rdBuf.empty())
		return ("");
	pos = rdBuf.find("\n");
	if (pos != std::string::npos) {

		std::string cmd = rdBuf.substr(0, pos);
		std::cout << "cmd: " << cmd << std::endl;

		rdBuf = rdBuf.substr(pos + 1);
		std::cout << "remain: " << rdBuf << std::endl;

		return (cmd);
	}
	return ("");
}

Server::Server(std::string port, std::string passwd, int fd)
	: _port(port), _passwd(passwd), _servfd(fd)
{
	// First entry in the _pollfds array is used for the listening socket
	struct pollfd servPoll;
	servPoll.fd = _servfd;
	servPoll.events = POLLIN;	// specify events of interest on fd
	_pollfds.push_back(servPoll);

	//
	this->commandMap["PASS"] = &Server::pass;
    this->commandMap["USER"] = &Server::user;
    this->commandMap["NICK"] = &Server::nick;
}

Server::~Server()
{
}

void	Server::start()
{
	int				nready;
	std::string		cmd;

	for ( ; ; ) {

		this->printpollfds();
		this->printClients();
		if (( nready = poll(&_pollfds[0], _pollfds.size(), -1)) == -1)
		{
			// close fds
			std::perror("poll");
			exit(1);
		}

		if (nready == 0)
			continue;

		if (_pollfds[0].revents & POLLIN) { // listening socket is readable

			handleNewConnection();			
			if (!--nready) continue;
		}


		for (size_t k = 1; k < _pollfds.size(); k++) { // check for returned events on other sockets

			 if (_pollfds[k].revents & (POLLHUP | POLLERR)) {	// client connection reset or error

				disconnectClient(k-1);
				k--;
				continue;
			}
			if (_pollfds[k].revents & (POLLIN )) {

				handleRead(k-1);

				// nready--;
			}
			// if (_pollfds[k].revents & POLLOUT) {
			cmd = getCommand(k-1);
			if (!cmd.empty()) {
				handleCommand(cmd, k-1);
			}
			// }
			// if (!nready) break;
		}

	}
}


void Server::printClients(void)
{
	std::vector<Client>::iterator it;

    // Define column headers
	std::cout << "----------------------------------------\n";
    std::cout << std::left << std::setw(5) << "id"
              << std::setw(10) << "fd"
              << std::setw(15) << "ip"
              << std::setw(10) << "port" << std::endl;

    // Print separator line
    std::cout << std::setfill('-') << std::setw(40) << "" << std::endl;
    std::cout << std::setfill(' ');

    // Print data rows
	int id = 0;
    for (it = _clients.begin(); it < _clients.end(); it++) {
        std::cout << std::left << std::setw(5) << id++
                  << std::setw(10) << it->getSockfd()
                  << std::setw(15) << it->getIPAddr()
                  << std::setw(10) << it->getPort() << std::endl;
    }
	std::cout << "\n\n";
}

void Server::printpollfds(void)
{
	std::vector<struct pollfd>::iterator it;

    // Define column headers
	std::cout << "----------------------------------------\n";
    std::cout << std::left << std::setw(5) << "id"
              << std::setw(10) << "fd"
              << std::setw(15)  << std::endl;

    // Print separator line
    std::cout << std::setfill('-') << std::setw(40) << "" << std::endl;
    std::cout << std::setfill(' ');

    // Print data rows
	int id = 0;
    for (it = _pollfds.begin(); it < _pollfds.end(); it++) {
        std::cout << std::left << std::setw(5) << id++
                  << std::setw(10) << it->fd << std::endl;
    }
	std::cout << "\n\n";
}
