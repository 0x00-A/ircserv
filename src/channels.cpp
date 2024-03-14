#include "Server.hpp"

void Server::createChannel(string channelName)
{
    Channel newChannel(channelName);
    this->_channels.push_back(newChannel);
}

void Server::joinChannel(Client &client, string channelName)
{
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getName() == channelName)
        {
            (void)client;
            // this->_channels[i].addClient(client);
            return;
        }
    }
    cerr << "Error: channel not found" << endl;
}

void Server::leaveChannel(Client &client, string channelName)
{
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getName() == channelName)
        {
            (void)client;
            // this->_channels[i].removeClient(client);
            return;
        }
    }
    cerr << "Error: channel not found" << endl;
}

Server::channelIter Server::doesChannelExist(const string &chan)
{
    for (channelIter it = _channels.begin(); it < _channels.end(); it++)
    {
        if (it->getName() == chan)
            return (it);
    }
	return (_channels.end());
}

Server::clientIter Server::doesUserExit(const string &nick)
{
    for (clientIter it = _clients.begin(); it < _clients.end(); it++)
    {
        if (it->getNick() == nick)
            return (it);
    }
	return (_clients.end());
}
