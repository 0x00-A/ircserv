#include "Server.hpp"

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

int Server::handleNewConnection()
{
	struct sockaddr_in	cliaddr;
	char				*ip;
	int					connfd;
	socklen_t			len;
	int					flags;

	len = sizeof(cliaddr);
	if ( (connfd = accept(_servfd, (SA *) (&cliaddr), &len)) == -1)	// use c++ cast
	{
		if (errno != EWOULDBLOCK)
		{
			std::perror("accept");
			return (-1);
		}
		return (1);
	}
	// get ip address
	if (!(ip = inet_ntoa(cliaddr.sin_addr)))		// returns a pointer to a static buffer inside the function
	{
		std::cerr << "inet_ntoa failed" << std::endl;
		close(connfd);
		return (1);
	}
	// Add the new client socket to _clients and _pollfds
	_clients.push_back(Client(ip, ntohs(cliaddr.sin_port), connfd));
	_pollfds.push_back((struct pollfd){.fd = connfd, .events = (POLLIN)});
	// set socket to be Non-blocking
	if ( (flags = fcntl(connfd, F_GETFL)) == -1
		|| fcntl(connfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::perror("fcntl");
		close(connfd);
		return (1);
	}
	std::cout << "client connected - fd: " << connfd << std::endl;
	return (0);
}

void Server::disconnectClient(int id)
{

	std::vector<Client>::iterator			cli_it;
	std::vector<struct pollfd>::iterator	poll_it;

	cli_it = _clients.begin() + id;
	poll_it = _pollfds.begin() + id + 1;

	std::cout << "client disconnected - fd: " << _pollfds[id+1].fd << std::endl;
	cli_it->close();
	_clients.erase(cli_it);
	_pollfds.erase(poll_it);
}

int Server::handleRead(int id)
{
	char		readbuf[512];
	ssize_t		bytesread;

	do
	{
		bytesread = read(_clients[id].getSockfd(), readbuf, sizeof(readbuf));
		if (bytesread < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				std::perror("read");
				return (-1);
			}
			return (0);
		}
		if (bytesread == 0)
		{
			return (-1);
		}
		// append to client read buffer
		readbuf[bytesread] = '\0';
		_clients[id].rdBuf() += std::string(readbuf);
		// send msg back to client
		std::string reply = "srv recved msg: " + std::string(readbuf);
		_clients[id].sdBuf() = reply;
		write(_clients[id].getSockfd(), reply.c_str(), reply.length());
		std::cout << _clients[id].getSockfd() << " rdbuf: " << _clients[id].rdBuf() << std::endl;
	} while (true);
}

void Server::handleWrite(int id)
{
	if (write(_clients[id].getSockfd(), _clients[id].sdBuf().c_str(), (_clients[id].sdBuf()).length()) == -1)
	{
		std::perror("write");
		return;
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

void	Server::start()
{
	bool	stat = true;
	std::vector<struct pollfd>::iterator it;
	int		ret;
	size_t	end;

	std::cout << "Server running" << std::endl;
	do
	{
		std::cout << "Polling ... [ connected clients: " << _clients.size() << " ]" << std::endl;
		if(poll(&_pollfds[0], _pollfds.size(), -1) == -1)
		{
			std::perror("poll"); break;
		}

		end = _pollfds.size();
		for (size_t i = 0; i < end; i++)
		{
			if (_pollfds[i].revents & POLLIN)
			{
				//
				if (_pollfds[i].fd == _servfd)
				{
					
					std::cout << "Server has incomming connection(s)" << std::endl;
					do
					{
						// accept incomming connections
						ret = handleNewConnection();
						if (ret == -1)
						{
							stat = false;
							break;
						}
					} while (ret != 1);
				}
				else
				{

					// handle existing connections
					std::cout << "fd " << _pollfds[i].fd << " is readable" << std::endl;
					ret = handleRead(i-1);
					if (ret == -1)
					{
						disconnectClient(i-1);
						i--;
						end--;
						continue;
					}
					std::string cmd = getCommand(i-1);
					if (!cmd.empty()) {
						handleCommand(cmd, i-1);
					}
				}
			}
			// else if (_pollfds[i].revents & POLLOUT)
			// {
			// 	// send reply to client
			// }
			// else if (_pollfds[i].revents & POLLERR)
			// {
			// 	std::cerr << "Server: polling failed" << std::endl;
			// }
		}
	} while (stat == true);
	closeAllOpenSockets();
}

void Server::closeAllOpenSockets(void)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		close(_pollfds[i].fd);
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
	std::cout << "\n";
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
	std::cout << "\n";
}
