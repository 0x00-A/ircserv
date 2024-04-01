#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{
    client.sdBuf().push(msg + "\r\n");
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}

void Server::broadcastMsg(Client &sender, const string &msg, const Channel &chan)
{
	if (chan.isUserInChannel(sender.getNick()) == false)
	{
            reply(sender, ERR_CANNOTSENDTOCHAN + " " + \
				sender.getNick() + " " + chan.getName() + " :Cannot send to channel");
	}
	else
	{
		for (clientIter it = _clients.begin(); it < _clients.end(); it++)
		{
			if (chan.isUserInChannel(it->getNick()))
			{
				if (it->getNick() == sender.getNick())
                    continue;
				reply(*it, msg);
			}
		}
	}
}

void Server::welcomeClient(Client &client)
{
	string	response;

	response  = RPL_WELCOME + " " + client.getNick() +  \
    " :Welcome to the " + _servname + " Network, " + client.identifier();
	reply(client, response);
	response = RPL_YOURHOST + " " + client.getNick() + \
    " :Your host is " + _servname + ", running version " + "version: 01";
	reply(client, response);
	response = RPL_CREATED + " " + client.getNick() + \
    " :This server was created " + getStartTime();
	reply(client, response);
    lusers(client);
    // :calcium.libera.chat 005 aaa CHANMODES=eIbq,k,flj,CFLMPQRSTcgimnprstuz CHANLIMIT=#:250 PREFIX=(ov)@+ MAXLIST=bqeI:100 MODES=4 NETWORK=Libera.Chat STATUSMSG=@+ CASEMAPPING=rfc1459 NICKLEN=16 MAXNICKLEN=16 CHANNELLEN=50 TOPICLEN=390 :are supported by this server
    motd(client);
}

