#include "Server.hpp"

string		Server::getMembers(Channel& ch)
{
    string response;

	std::set<string>  members = ch.getUserList();
	std::set<string>::iterator it = members.begin();
	for ( ; it != members.end(); it++)
	{
		response += " " + *it;
	}
	if (response[0] == ' ')
		response.erase(0, 1);
	return response;

}

string		channelInfo(const string& nick, const string& nameCha)
{
    string response;
	response = ":ft_irc.1337.ma " + itos(RPL_NAMREPLY) + " " + nick + " @ " +  nameCha + " :";
	return (response);

}
void Server::channelWelcomeMessages(Client &client, Channel& ch)
{


	reply(client, client.identifier() + " JOIN " + ch.getName());
	// reply(client, ":ft_irc.1337.ma MODE " + ch.getName() + " " + ch.channelModeIs());
	if (ch.isUserOperator(client.getNick()))
		reply(client, client.identifier() + " MODE " + ch.getName() + " " + "+t");

	showChannelNames(client, ch);
}

void Server::joinedAChannel(Client& client, Channel& channel)
{
	string 						response;
	std::set<string>	users;
	clientIter					cliIter;

	users = channel.getUserList();
    response = client.identifier() + " JOIN " + channel.getName();
	for (std::set<string>::iterator it = users.begin(); it != users.end(); it++)
	{
		cliIter = getClientIterator(*it);
		if (cliIter != _clients.end())
		{
			if (client.getNick() == *it) continue;
			reply(*cliIter, response);
		}
	}
}

void Server::joinChannel(Client &client, std::pair<string, string> channel)
{
	string response;
	std::map<string, bool> invitees;

	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == channel.first)
		{
			if (_channels[i].isUserInChannel(client.getNick())) return;
			if (_channels[i].hasInvite())
			{
				invitees = client.getInvitees();
				std::map<string, bool>::iterator itInvt =  invitees.find(channel.first);
				if (itInvt != invitees.end())
				{
					if (itInvt->second == false)
					{
						response =  (":ft_irc.1337.ma "  + itos(ERR_INVITEONLYCHAN) + " " + client.getNick()  + " " + _channels[i].getName() + " :Cannot join channel, you must be invited (+i)");
						reply(client, response);
						return ;
					}
				}
			}
			if (_channels[i].hasPasskey()  && !_channels[i].hasInvite())
			{
				if (_channels[i].getPasskey() != channel.second)
				{
					response =  (":" + client.getIPAddr() + " "  + itos(ERR_BADCHANNELKEY) + client.getNick() \
					+ " " + channel.first + " :Cannot join channel (+K) - bad key");
					reply(client, response);
					return ;
				}
			}
			if (_channels[i].hasUserLimit())
			{
				if (_channels[i].getUserLimit() <= _channels[i].getSize())
				{
					response =  (":ft_irc.1337.ma "  + itos(ERR_CHANNELISFULL) + _channels[i].getName() + " :Cannot join channel (+l)");
					reply(client, response);
					return ;
				}
			}
			this->_channels[i].joinUser(client.getNick());
			client.addChannels(channel.first);
			channelWelcomeMessages(client, _channels[i]);
			joinedAChannel(client, _channels[i]);
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

	prefix = channelInfo(client.getNick(), ch.getName());
	users = ch.getUserList();
	size_t newlen;
	response = prefix;
	for (std::set<string>::reverse_iterator it = users.rbegin(); it != users.rend(); it++)
	{
		if (response != prefix)
			newlen = response.size() + 1 + it->size();
		else
			newlen = response.size() + it->size();
		if (newlen > 510)
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
	
	response = ":ft_irc.1337.ma " + itos(RPL_ENDOFNAMES) + " " + client.getNick() + " " +  ch.getName() + " :End of /NAMES list.";
	reply(client, response);
}
