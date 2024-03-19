#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{

    string response = msg + "\r\n";

    client.sdBuf().push(response);
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}

void Server::broadcastMsg(Client &sender, const string &msg, const Channel &chan)
{

	string response;
	if (chan.isUserInChannel(sender.getNick()) == false)
	{
			response = ":ft_irc.1337.ma " + intToString(ERR_CANNOTSENDTOCHAN) + \
             " " +  sender.getNick() + " " + chan.getName() + " :Cannot send to channel";
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
