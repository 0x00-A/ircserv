#include "Server.hpp"

// void Server::addChannel(string channelName, Client &client)
// {
//     Channel newChannel(channelName);
//     this->_channels.push_back(newChannel);
//     this->_channels.back().joinUser(client.getNick());
//     this->_channels.back().setChannelOperator(client.getNick());
// }

void Server::joinChannel(Client &client, std::pair<string, string> channel)
{
    string response;

    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getName() == channel.first)
        {
            if (_channels[i].hasUserLimit())
            {
                if (_channels[i].getUserLimit() < _channels[i].getSize())
                {
                    throw (":ft_irc.1337.ma "  + to_string(ERR_CHANNELISFULL) + _channels[i].getName() + " :Cannot join channel (+l)");
                }
            }

            if (_channels[i].hasPasskey())
            {
                // ERR_BADCHANNELKEY
                //          "<channel> :Cannot join channel (+k)"
                if (_channels[i].getPasskey() != channel.second)
                {
                    throw (":" + client.getIPAddr() + " " + client.getNick() + " " + channel.first + " :Cannot join channel (+K) - bad key");
                }
            }
            this->_channels[i].joinUser(client.getNick());
            return ;
        }
    }
    this->_channels.push_back(Channel(channel.first, client.getNick()));
}

// void Server::leaveChannel(Client &client, string channelName)
// {
//     for (size_t i = 0; i < this->_channels.size(); i++)
//     {
//         if (this->_channels[i].getName() == channelName)
//         {
//             (void)client;
//             // this->channels[i].removeClient(client);
//             return;
//         }
//     }
//     cerr << "Error: channel not found" << endl;
// }

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
