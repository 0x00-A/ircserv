#include "Server.hpp"

void Server::broadcastToJoinedChannels(Client& client, string &msg)
{
    std::set<string>    joinedChans;
	std::set<string>	tempUsers;
	Channel				tmpChannel;

    joinedChans = client.getChannels();
    std::set<string>::iterator it = joinedChans.begin();
    for ( ; it != joinedChans.end(); it++)
    {
        channelIter itCha = doesChannelExist(*it);
        if (itCha != _channels.end())
        {
			std::set<string> users = itCha->getUserList();
			for (std::set<string>::iterator it = users.begin(); it != users.end(); it++)
			{
				 tempUsers.insert(*it);
			}
        }
    }
	tmpChannel.setUsers(tempUsers);
	this->broadcastMsg(client, msg, tmpChannel);
}


void Server::channelWelcomeMessages(Client &client, Channel& ch)
{
	reply(client, client.identifier() + " JOIN " + ch.getName());

	if (!ch.getTopic().empty())
	{
		reply(client, _servname + " " + RPL_TOPIC + " " + client.getNick() + " " +  ch.getName() + " :" + ch.getTopic());
		reply(client, _servname + " " + RPL_TOPICWHOTIME + " " + client.getNick() + " " + ch.getName()  \
			+ " "  + client.identifier() + " " + itos(ch.getTimeOfTopic()));
	}
	if (ch.isUserOperator(client.getNick()))
		reply(client, client.identifier() + " MODE " + ch.getName() + " " + "+t");

	showChannelNames(client, ch);
}

void Server::joinedAChannel(Client& client, Channel& channel)
{
	string 						response;
	std::set<string>			users;
	clientIter					cliIter;

	users = channel.getUserList();
    response = client.identifier() + " JOIN " + channel.getName();
	for (std::set<string>::iterator it = users.begin(); it != users.end(); it++)
	{
		cliIter = getClientIterator(*it);
		if (cliIter != _clients.end())
		{
			if (client.getNick() == *it)
				continue;
			reply(*cliIter, response);
		}
	}
}

void Server::joinChannel(Client &client, std::pair<string, string> channel)
{
	string response;

	if (client.getChannels().size() >= MAXCHANNELS)
	{
		throw (_servname + " " + ERR_TOOMANYCHANNELS + " " + client.getNick() + " " + channel.first + \
				" :You have joined too many channels");
	}
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == channel.first)
		{
			if (_channels[i].isUserInChannel(client.getNick())) return;
			if (_channels[i].hasInvite())
			{
				if (!client.isInvitedToChannel(channel.first)) 
				{
					response =  (_servname + " " + ERR_INVITEONLYCHAN + " " + client.getNick()  + " " + \
						_channels[i].getName() + " :Cannot join channel, you must be invited (+i)");
					reply(client, response);
					return ;
				}
			}
			if (_channels[i].hasPasskey()  && !client.isInvitedToChannel(channel.first))
			{
				if (_channels[i].getPasskey() != channel.second)
				{
					response =  (_servname + " "  + ERR_BADCHANNELKEY + " " + client.getNick() + " "\
					+ _channels[i].getName() + " :Cannot join channel, you need the correct key (+k)");
					reply(client, response);
					return ;
				}
			}
			if (_channels[i].hasUserLimit() && !client.isInvitedToChannel(channel.first))
			{
				if (_channels[i].getUserLimit() <= _channels[i].getSize())
				{
					response =  (_servname + " " + ERR_CHANNELISFULL + " " + client.getNick() + " " + \
					_channels[i].getName() + " :Cannot join channel, Channel is full (+l)");
					reply(client, response);
					return ;
				}
			}
			this->_channels[i].joinUser(client.getNick());
			client.addChannels(channel.first);
			channelWelcomeMessages(client, _channels[i]);
			joinedAChannel(client, _channels[i]);
			client.uninviteFromChannel(channel.first);
			return ;
		}
	}
	this->_channels.push_back(Channel(channel.first, client.getNick()));
	client.addChannels(channel.first);
	channelWelcomeMessages(client, _channels.back());
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


Server::clientIter Server::doesUserExit(const string nick)
{
	for (clientIter it = _clients.begin(); it < _clients.end(); it++)
	{
		if (it->getNick() == nick || ("@" + it->getNick()) == nick)
			return (it);
	}
	return (_clients.end());
}

void Server::removeUserFromChannel(const string user, const string chan)
{
	channelIter it = doesChannelExist(chan);
	if (it != _channels.end())
	{
		it->partUser(user);
		if (it->getSize() <= 0)
		{
			_channels.erase(it);
		}
	}
}

void Server::exitUserFromChannels(clientIter cli)
{
	std::set<string> chans = cli->getChannels();
	for (std::set<string>::iterator it = chans.begin(); it != chans.end(); it++)
	{
		removeUserFromChannel(cli->getNick(), *it);
	}
}

void Server::showChannelNames(Client &client, Channel &ch)
{
    string		    	response;
	string 			    prefix;
	std::set<string>	users;

	prefix = _servname + " " + RPL_NAMREPLY + " " + client.getNick() + " @ " +  ch.getName() + " :";
	users = ch.getUserList();
	size_t newlen;
	response = prefix;
	for (std::set<string>::reverse_iterator it = users.rbegin(); it != users.rend(); it++)
	{
		if (response != prefix)
			newlen = response.size() + 1 + it->size();
		else
			newlen = response.size() + it->size();
		if (newlen > RD_BUF_SIZE - 2)
		{
			reply(client, response);
			response = prefix;
			it--;
			continue;
		}
		if (response != prefix)
			response += " " + *it;
		else
			response += *it;
	}
	if (response != prefix)
        reply(client, response);
	response = _servname + " " + RPL_ENDOFNAMES + " " + client.getNick() + " " +  ch.getName() + \
				" :End of /NAMES list.";
	reply(client, response);
}
