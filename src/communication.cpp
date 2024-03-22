#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{
    client.sdBuf().push(msg + "\r\n");
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}

void Server::broadcastMsg(Client &sender, const string &msg, const Channel &chan)
{

	string response;
	if (chan.isUserInChannel(sender.getNick()) == false)
	{
			response = ":ft_irc.1337.ma " + intToString(ERR_CANNOTSENDTOCHAN) + " " +  sender.getNick() + " " + chan.getName() + " :Cannot send to channel";
            reply(sender, response);
	}
	else
	{
		for (clientIter it = _clients.begin(); it < _clients.end(); it++)
		{
			if (chan.isUserInChannel(it->getNick()))
			{
				if (it->getNick() == sender.getNick()) continue;
				response = ":"  + sender.getNick() + "!~" + sender.getUsername()  + "@" + \
						sender.getIPAddr() + " PRIVMSG " + chan.getName() + " :" +  msg;
				reply(*it, response);
			}
		}
	}
}

void Server::welcomeClient(Client &client)
{
	string	response;

	response  = ":ft_irc.1337.ma " + intToString(RPL_WELCOME) + " :Welcome to the " + "ft_irc.1337.ma" + " Network, " + client.getNick() + "!" + client.getUsername() + "@" + client.getIPAddr();
	reply(client, response);
	response = ":ft_irc.1337.ma " + intToString(RPL_YOURHOST) + " :Your host is " + "ft_irc.1337.ma" + ", running version " + "version01";
	reply(client, response);
	response = ":ft_irc.1337.ma " + intToString(RPL_CREATED) + " :This server was created " + getStartTime();
	reply(client, response);
}