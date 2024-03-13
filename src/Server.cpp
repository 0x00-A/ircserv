#include "Server.hpp"

Server::Server(const string& port, const string& passwd)
	: _port(port), _passwd(passwd)
{
	// socket part
	_socket.listenSocket(_port);
	_socket.setSocketNonBlocking();
	_servfd = _socket.getfd();
	// First entry in the _pollfds array is used for the listening socket
	struct pollfd servPoll;
	servPoll.fd = _servfd;
	servPoll.events = POLLIN;	// specify events of interest on fd
	_pollfds.push_back(servPoll);

	//
	this->commandMap["pass"] = &Server::pass;
    this->commandMap["user"] = &Server::user;
    this->commandMap["nick"] = &Server::nick;
    this->commandMap["quit"] = &Server::quit;
    this->commandMap["join"] = &Server::join;
    this->commandMap["privmsg"] = &Server::privmsg;
    this->commandMap["mode"] = &Server::mode;
}

Server::~Server()
{
}

void Server::broadcastMsg(const Client &sender, const string &msg, const Channel &chan)
{
	for (clientIter it = _clients.begin(); it < _clients.end(); it++)
	{
		if (chan.isUserInChannel(sender.getNick()))
		{
			// send message
			(void)msg;
		}
	}
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
			perror("accept");
		return (1);
	}
	// get ip address
	if (!(ip = inet_ntoa(cliaddr.sin_addr)))		// inet_ntoa returns a pointer to a static buffer inside the function
	{
		cerr << "inet_ntoa failed" << endl;
		close(connfd);
		return (1);
	}
	// set socket to be Non-blocking
	if ( (flags = fcntl(connfd, F_GETFL)) == -1
		|| fcntl(connfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl");
		close(connfd);
		return (1);
	}
	// Add the new client socket to _clients and _pollfds
	_clients.push_back(Client(ip, ntohs(cliaddr.sin_port), connfd));
	_pollfds.push_back((struct pollfd){.fd = connfd, .events = (POLLIN)});

	cout << "client connected - fd: " << connfd << endl;
	return (0);
}

void Server::disconnectClient(int id)
{

	clientIter			cli_it;
	pollfdIter			poll_it;

	cli_it = _clients.begin() + id;
	poll_it = _pollfds.begin() + id + 1;

	cout << "client disconnected - fd: " << _pollfds[id+1].fd << endl;
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
				perror("read");
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
		_clients[id].rdBuf() += string(readbuf);
		// cout << "fd " << _clients[id].getSockfd() << " rdbuf: " << _clients[id].rdBuf() << endl;
	} while (true);
}

int	Server::handleWrite(int id)
{
	ssize_t sent_data;
	std::queue<string>& buffer = _clients[id].sdBuf();

	// just for debuging
	if (buffer.empty())
	{
		cout << "send Buffer empty returning - fd: " << _clients[id].getSockfd() << endl;
		return (0);
	}

	while (!buffer.empty())
	{
		string	data = buffer.front();

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
	cout << "Done sending data to client - fd: " << _clients[id].getSockfd() << endl;
	_pollfds[id+1].events = POLLIN;
	return (0);
}

string Server::getCommand(int id)
{
	string& rdBuf = _clients[id].rdBuf();
	size_t	pos;

	if (rdBuf.empty())
		return ("");
	pos = rdBuf.find("\n");
	if (pos != string::npos) {

		string cmd = rdBuf.substr(0, pos);
		cout << "cmd: " << cmd << endl;

		rdBuf = rdBuf.substr(pos + 1);
		// cout << "remain: " << rdBuf << endl;

		return (cmd);
	}
	return ("");
}

void	Server::start()
{
	pollfdIter	it;
	int			ret;
	string		cmd;


	cout << "Server running" << endl;
	while (true)
	{
		// printClients();
		cout << "Polling ... [ connected clients: " << _clients.size() << " ]" << endl;
		if(poll(&_pollfds[0], _pollfds.size(), -1) == -1)
		{
			perror("poll"); break;
		}
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)	// data sent from client
			{
				//
				if (_pollfds[i].fd == _servfd)
				{
					
					cout << "Server has incomming connection(s)" << endl;
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
					cout << "fd " << _pollfds[i].fd << " is readable" << endl;
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
			
				cout << "fd " << _pollfds[i].fd << " is writeable" << endl;
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
					cerr << "Server: polling failed" << endl;
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
			cout << "Done cleaning all sockets" << endl;
		}
	}
}

void Server::closeAllOpenSockets(void)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		close(_pollfds[i].fd);
	}
	cout << "Done closing all sockets" << endl;
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
	cout << "----------------------------------------\n";
    cout << std::left << std::setw(5) << "id"
              << std::setw(10) << "fd"
              << std::setw(15) << "ip"
              << std::setw(10) << "port" << endl;

    // Print separator line
    cout << std::setfill('-') << std::setw(40) << "" << endl;
    cout << std::setfill(' ');

    // Print data rows
	// int id = 0;
    for (it = _clients.begin(); it < _clients.end(); it++) {
        cout << std::left << std::setw(5) << getIndexOfClient(it)
                  << std::setw(10) << it->getSockfd()
                  << std::setw(15) << it->getIPAddr()
                  << std::setw(10) << it->getPort() << endl;
    }
	cout << "\n";
}

void Server::printpollfds(void)
{
	pollfdIter it;

    // Define column headers
	cout << "----------------------------------------\n";
    cout << std::left << std::setw(5) << "id"
              << std::setw(10) << "fd"
              << std::setw(15)  << endl;

    // Print separator line
    cout << std::setfill('-') << std::setw(40) << "" << endl;
    cout << std::setfill(' ');

    // Print data rows
	int id = 0;
    for (it = _pollfds.begin(); it < _pollfds.end(); it++) {
        cout << std::left << std::setw(5) << id++
                  << std::setw(10) << it->fd << endl;
    }
	cout << "\n";
}
