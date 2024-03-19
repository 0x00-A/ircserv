#include "Server.hpp"


string Server::clientIdentifier(Client &client, string& ch, Channel& channel)
{
    string response;

    response = ":" + client.getNick() + "!~" + client.getUsername() + "@" + client.getIPAddr() + " JOIN " + ch + "\n" + \
	":ft_irc.1337.ma MODE " + ch + " " + channel.channelModeIs() + "\n" + \
	":ft_irc.1337.ma " + intToString(RPL_NAMREPLY) + " " + client.getNick() + " @ " +  ch + ":"  + client.getNick() + " :@" + channel.getAdmin() + "\n" + \
	":ft_irc.1337.ma " + intToString(RPL_ENDOFNAMES) + " " + client.getNick() + " " +  ch + " :End of /NAMES list";
	return response;
}

void Server::joinedAChannel(Client& client, Channel& channel)
{
	string response;
	std::set<string>	operators;

	operators = channel.getOperatorList();
    response = ":" + client.getNick() + "!~" + client.getUsername() + "@" + client.getIPAddr() + " JOIN " + channel.getName();
	for (std::set<string>::iterator it = operators.begin(); it != operators.end(); it++)
	{	
		for (size_t j = 0; j < _clients.size() ; j++)
		{
			if (*it == _clients[j].getNick())
			{
				reply(_clients[j], response);
			}
		}
	}
}

void Server::joinChannel(Client &client, std::pair<string, string> channel)
{
	string response;

	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == channel.first)
		{
			if (_channels[i].isUserInChannel(client.getNick())) return;
			if (_channels[i].hasInvite())
			{
				/// here for check client invite after add cmd invite 
			}
			if (_channels[i].hasPasskey()  && !_channels[i].hasInvite())
			{
				if (_channels[i].getPasskey() != channel.second)
				{
					response =  (":" + client.getIPAddr() + " "  + intToString(ERR_BADCHANNELKEY) + client.getNick() \
					+ " " + channel.first + " :Cannot join channel (+K) - bad key");
					reply(client, response);
					return ;
				}
			}
			if (_channels[i].hasUserLimit())
			{
				if (_channels[i].getUserLimit() < _channels[i].getSize())
				{
					response =  (":ft_irc.1337.ma "  + intToString(ERR_CHANNELISFULL) + _channels[i].getName() + " :Cannot join channel (+l)");
					reply(client, response);
					return ;
				}
			}
			this->_channels[i].joinUser(client.getNick());
			reply(client, clientIdentifier(client, channel.first, _channels[i]));
			joinedAChannel(client, _channels[i]);
			return ;
		}
	}
	this->_channels.push_back(Channel(channel.first, client.getNick()));
	reply(client, clientIdentifier(client, channel.first, _channels.back()));
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
