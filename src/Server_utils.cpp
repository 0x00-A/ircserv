#include "Server.hpp"

void Server::setStartTime(void)
{
	std::time_t res = std::time(NULL);

    char *timePtr = std::ctime(&res);
    if (!timePtr)
    {
        cerr << "error time" << endl;
        _startTime = "";
    }
    else
    {
        _startTime = timePtr;
		_startTime.erase(_startTime.size() - 1, 1);
    }
}

string Server::getStartTime(void) const
{
    return (_startTime);
}

void Server::cleanDisconnectdClients()
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == -1)
		{
			disconnectClient(i - 1);
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
	for (clientIter it = _clients.begin(); it < _clients.end(); it++)
	{
		if (it->getSockfd() == cli.getSockfd())
			return (it);
	}
	return (_clients.end());
}

Server::clientIter Server::getClientIterator(const string &nick)
{
	clientIter it;
	for (it = _clients.begin(); it < _clients.end(); it++)
	{
		if ((it->getNick() == nick) || ("@" + it->getNick()) == nick)
			return (it);
	}
	return (it);
}

int Server::getIndexOfClient(const clientIter& currIter)
{
    clientIter	beginIter = _clients.begin();
	return (currIter - beginIter);
}

int Server::getUnknownConnections()
{
	int unknownConnections;

	unknownConnections = 0;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].isConnected() == false)
		{
			unknownConnections++;
		}
	}
    return (unknownConnections);
}

int Server::getKnownConnections()
{
	int knownConnections;

	knownConnections = 0;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].isConnected() == true)
		{
			knownConnections++;
		}
	}
    return (knownConnections);
}

int Server::getExistingChannels()
{
	int channelsExisting;

	channelsExisting = 0;
	for (size_t i = 0; i < _channels.size(); i++)
	{
		channelsExisting++;
	}
    return (channelsExisting);
}
