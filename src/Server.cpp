#include "Server.hpp"

int	isServerUp = true;

Server::Server(const string& port, const string& passwd)
	: _port(port), _passwd(passwd)
{
	parseargs();
	// socket part
	_servname = ":ft_irc.1337.ma";
	_socket.bindSocket(_port);
	_socket.listenSocket();
	_socket.setSocketNonBlocking();
	_servfd = _socket.getfd();
	// First entry in the _pollfds array is used for the listening socket
	struct pollfd servPoll;
	servPoll.fd = _servfd;
	servPoll.events = POLLIN;	// events of interest on fd
	_pollfds.push_back(servPoll);

	this->commandMap["PASS"] = &Server::pass;
    this->commandMap["USER"] = &Server::user;
    this->commandMap["NICK"] = &Server::nick;
    this->commandMap["QUIT"] = &Server::quit;
    this->commandMap["JOIN"] = &Server::join;
    this->commandMap["PRIVMSG"] = &Server::privmsg;
    this->commandMap["NOTICE"] = &Server::notice;
    this->commandMap["NAMES"] = &Server::names;
    this->commandMap["MODE"] = &Server::mode;
    this->commandMap["KICK"] = &Server::kick;
    this->commandMap["INVITE"] = &Server::invite;
    this->commandMap["TOPIC"] = &Server::topic;
    this->commandMap["LUSERS"] = &Server::lusers;
    this->commandMap["MOTD"] = &Server::motd;
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

	len = sizeof(cliaddr);
	if ( (connfd = accept(_servfd, reinterpret_cast<SA *>(&cliaddr), &len)) == -1)
	{
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			perror("accept");
		return (1);
	}
	// get ip address
	if (!(ip = inet_ntoa(cliaddr.sin_addr)))
	{
		cerr << "inet_ntoa failed" << endl;
		close(connfd);
		return (1);
	}
	// set socket to be Non-blocking
	if (fcntl(connfd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		close(connfd);
		return (1);
	}
	// Add the new client socket to _clients and _pollfds
	_clients.push_back(Client(ip, ntohs(cliaddr.sin_port), connfd));
	_pollfds.push_back((struct pollfd){.fd = connfd, .events = (POLLIN), .revents = 0});
	return (0);
}

void Server::disconnectClient(int id)
{
	clientIter			cli_it;
	pollfdIter			poll_it;

	cli_it = _clients.begin() + id;
	poll_it = _pollfds.begin() + id + 1;
	exitUserFromChannels(cli_it);
	_clients.erase(cli_it);
	_pollfds.erase(poll_it);
}

int Server::handleRead(int id)
{
	char		readbuf[RD_BUF_SIZE + 1];
	ssize_t		bytesread;

	do
	{
		bytesread = read(_clients[id].getSockfd(), readbuf, sizeof(readbuf) - 1);
		if (bytesread < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				perror("read...");
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
		if (_clients[id].rdBuf().length() > RD_BUF_SIZE)
		{
			_clients[id].rdBuf().clear();
			reply (_clients[id], _servname + " " + ERR_INPUTTOOLONG + " " + _clients[id].getNick() + \
				" :Input line was too long");
			return (0);
		}
	} while (_clients[id].rdBuf().find("\n") == string::npos);
	return (0);
}

int	Server::handleWrite(int id)
{
	ssize_t sent_data;
	std::queue<string>& buffer = _clients[id].sdBuf();

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
	cout << "Done sending data to client - fd: " << _clients[id].getSockfd()  << " send_size: " << sent_data << endl;
	_pollfds[id+1].events = POLLIN;
	return (0);
}

void	Server::run()
{
	pollfdIter	it;
	int			ret;
	string		cmd;

	cout << "Server running" << endl;
	setStartTime();
	while (isServerUp)
	{
		if(poll(_pollfds.data(), _pollfds.size(), -1) == -1)
		{
			if (errno == EINTR) // (interrupted system call)
        		continue;
			perror("poll");
			break;
		}
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)	// data sent from client
			{
				if (_pollfds[i].fd == _servfd)
				{
					
					do
					{
						// accept incomming connections
						ret = handleNewConnection();

					} while (ret != 1);
				}
				else
				{
					// handle existing connections
					ret = handleRead(i-1);
					if (ret == -1)
					{
						quit(_clients[i-1]);
					}
					else
					{
						while ((cmd = getCommand(i-1)) != "")
						{
							try
							{
								handleCommand(cmd, i-1);
							}
							catch ( string& res )
							{
								reply(_clients[i-1], res);
							}
						}
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
				ret = handleWrite(i-1);
				if (ret == -1)
				{
					quit(_clients[i-1]);
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
					quit(_clients[i-1]);
				}
			}
		}
		cleanDisconnectdClients();
	}
	closeAllOpenSockets();
}
