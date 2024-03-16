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
            if (_channels[i].hasInvite())
            {
                /// here for check client invite after salat dohr
            }
            if (_channels[i].hasPasskey()  && !_channels[i].hasInvite())
            {
                if (_channels[i].getPasskey() != channel.second)
                {
                    response =  (":" + client.getIPAddr() + " "  + to_string(ERR_BADCHANNELKEY) + client.getNick() \
                    + " " + channel.first + " :Cannot join channel (+K) - bad key");
                    reply(client, response);
                    return ;
                }
            }
            if (_channels[i].hasUserLimit())
            {
                if (_channels[i].getUserLimit() < _channels[i].getSize())
                {
                    response =  (":ft_irc.1337.ma "  + to_string(ERR_CHANNELISFULL) + _channels[i].getName() + " :Cannot join channel (+l)");
                    reply(client, response);
                    return ;
                }
            }
            this->_channels[i].joinUser(client.getNick());
            return ;
        }
    }
    this->_channels.push_back(Channel(channel.first, client.getNick()));

    response = ":" + client.getNick() + "!~" + client.getUsername() + "@" + client.getIPAddr() + " JOIN " + channel.first;
    reply(client, response);
    response = ":ft_irc.1337.ma MODE " + channel.first + " +" + _channels.back().getModes();
    reply(client, response);
    response = ":ft_irc.1337.ma " + to_string(RPL_NAMREPLY) + " " + client.getNick() + " @ " +  channel.first + " :@" + client.getNick();
    reply(client, response);
    response = ":ft_irc.1337.ma " + to_string(RPL_ENDOFNAMES) + " " + client.getNick() + " " +  channel.first + " :End of /NAMES list";
    reply(client, response);
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
