#include "Server.hpp"

int	Server::_idxCounter = 0;

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
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			std::perror("accept");
		return (1);
	}
	// get ip address
	if (!(ip = inet_ntoa(cliaddr.sin_addr)))		// inet_ntoa returns a pointer to a static buffer inside the function
	{
		std::cerr << "inet_ntoa failed" << std::endl;
		close(connfd);
		return (1);
	}
	// set socket to be Non-blocking
	if ( (flags = fcntl(connfd, F_GETFL)) == -1
		|| fcntl(connfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::perror("fcntl");
		close(connfd);
		return (1);
	}
	// Add the new client socket to _clients and _pollfds
	_clients.push_back(Client(ip, ntohs(cliaddr.sin_port), connfd));
	_pollfds.push_back((struct pollfd){.fd = connfd, .events = (POLLIN)});

	std::cout << "client connected - fd: " << connfd << std::endl;
	return (0);
}

void Server::disconnectClient(int id)
{

	clientIter			cli_it;
	pollfdIter			poll_it;

	cli_it = _clients.begin() + id;
	poll_it = _pollfds.begin() + id + 1;

	std::cout << "client disconnected - fd: " << _pollfds[id+1].fd << std::endl;
	cli_it->closeSocket();
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
			if (errno != EWOULDBLOCK && errno != EAGAIN)
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
		// std::cout << "fd " << _clients[id].getSockfd() << " rdbuf: " << _clients[id].rdBuf() << std::endl;
	} while (true);
}

int	Server::handleWrite(int id)
{
	ssize_t sent_data;
	std::queue<std::string>& buffer = _clients[id].sdBuf();

	// just for debuging
	if (buffer.empty())
	{
		std::cout << "send Buffer empty returning - fd: " << _clients[id].getSockfd() << std::endl;
		return (0);
	}

	while (!buffer.empty())
	{
		std::string	data = buffer.front();

		sent_data = write(_clients[id].getSockfd(), data.c_str(), data.length());
		if (sent_data < 0)
		{
			/************************************************************************/
			/*  if a socket file descriptor is set to non-blocking mode and          */
			/*  you attempt to write() to it when the send buffer is full, write()  */
			/*  will fail and return -1, and errno will be set to EWOULDBLOCK       */
			/************************************************************************/
			if (errno != EWOULDBLOCK)
			{
				perror("write");
				return(-1);
			}
			return (0);
		}

		// if part of data was sent
		if (static_cast<size_t>(sent_data) < data.length())
		{
			buffer.front() = data.substr(sent_data);
			continue;
		}
		// if all data was sent
		buffer.pop();
	}
	std::cout << "Done sending data to client - fd: " << _clients[id].getSockfd() << std::endl;
	_pollfds[id+1].events = POLLIN;
	return (0);
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
		// std::cout << "remain: " << rdBuf << std::endl;

		return (cmd);
	}
	return ("");
}

void	Server::start()
{
	pollfdIter it;
	int			ret;
	std::string	cmd;


	std::cout << "Server running" << std::endl;
	while (true)
	{
		// printClients();
		std::cout << "Polling ... [ connected clients: " << _clients.size() << " ]" << std::endl;
		if(poll(&_pollfds[0], _pollfds.size(), -1) == -1)
		{
			std::perror("poll"); break;
		}
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)	// data sent from client
			{
				//
				if (_pollfds[i].fd == _servfd)
				{
					
					std::cout << "Server has incomming connection(s)" << std::endl;
					do
					{
						// accept incomming connections
						ret = handleNewConnection();
						// if (ret == 0)
						// {
						// 	_clients[_clients.size() - 1].setIndex(_clients.size() - 1);
						// }

					} while (ret != 1);
				}
				else
				{
					// handle existing connections
					std::cout << "fd " << _pollfds[i].fd << " is readable" << std::endl;
					ret = handleRead(i-1);
					if (ret == -1)
					{
						disconnectClient(--i);
						continue;
					}
					while ((cmd = getCommand(i-1)) != "")
					{
						handleCommand(cmd, i-1);
					}
				}
			}
			if (_pollfds[i].revents & POLLOUT)	// Client can recieve data
			{				
				/******************************************************************************/
				/*  Set POLLOUT only if you have data to send to the other end.               */
				/*  This is because most of the time, sockets are writable, and POLLOUT will  */
				/*  almost always be set if you ask for it. So, asking for POLLOUT when you   */
				/*  don’t have anything to write would just waste CPU cycles. 	              */
				/******************************************************************************/
			
				std::cout << "fd " << _pollfds[i].fd << " is writeable" << std::endl;
				ret = handleWrite(i-1);
				if (ret == -1)
				{
					disconnectClient(--i);
					continue;
				}
			}
			if (_pollfds[i].revents & POLLERR)	// client Connection reset (RST) 
			{	
				if (_pollfds[i].fd == _servfd)
				{
					std::cerr << "Server: polling failed" << std::endl;
					break;
				}
				else
				{
					disconnectClient(--i);
				}
			}
		}
		cleanUnusedClients();
	}
	closeAllOpenSockets();
}

void Server::cleanUnusedClients()
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == -1)
		{
			disconnectClient(i - 1);
			std::cout << "Done cleaning all sockets" << std::endl;
		}
	}
}

void Server::closeAllOpenSockets(void)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		close(_pollfds[i].fd);
	}
	std::cout << "Done closing all sockets" << std::endl;
}

int Server::getIndexOfClient(const Client &cli)
{
    clientIter	beginIter = _clients.begin();

    return (getClientIterator(cli) - beginIter);
}

Server::clientIter Server::getClientIterator(const Client &cli)
{
    return (std::find(_clients.begin(), _clients.end(), cli));
}

int Server::getIndexOfClient(const clientIter& currIter)
{
    clientIter	beginIter = _clients.begin();
	// return (std::distance(currIter, beginIter));
	return (currIter - beginIter);
}

void Server::printClients(void)
{
	clientIter it;

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
	// int id = 0;
    for (it = _clients.begin(); it < _clients.end(); it++) {
        std::cout << std::left << std::setw(5) << getIndexOfClient(it)
                  << std::setw(10) << it->getSockfd()
                  << std::setw(15) << it->getIPAddr()
                  << std::setw(10) << it->getPort() << std::endl;
    }
	std::cout << "\n";
}

void Server::printpollfds(void)
{
	pollfdIter it;

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
