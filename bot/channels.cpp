#include "IRCbot.hpp"

void IRCbot::updateChannels(std::vector<string> &tokens)
{
	if (tokens[1] == "QUIT")
	{
		for (chanIt it = _channels.begin(); it < _channels.end(); it++)
		{
			string nick = getUserNick(tokens[0]);
			it->removeUser(nick);
		}
	}
	if (tokens[1] == "NICK")
	{
		for (chanIt it = _channels.begin(); it < _channels.end(); it++)
		{
			string nick = getUserNick(tokens[0]);
			it->updateUserNick(nick, tokens[2]);
		}
	}
}

void	IRCbot::joinChannel( string& chan )
{
	sendReply("JOIN " + chan);
	_channels.push_back(Channel(chan));
}

IRCbot::chanIt IRCbot::getChanIt(string &name)
{
	for (chanIt it = _channels.begin(); it < _channels.end(); it++)
	{
		if (it->_name == name)
			return (it);
	}
	return (_channels.end());
}
