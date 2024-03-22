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
            reply(sender, ":ft_irc.1337.ma " + itos(ERR_CANNOTSENDTOCHAN) + " " + \
				sender.getNick() + " " + chan.getName() + " :Cannot send to channel");
	}
	else
	{
		for (clientIter it = _clients.begin(); it < _clients.end(); it++)
		{
			if (chan.isUserInChannel(it->getNick()))
			{
				if (it->getNick() == sender.getNick()) continue;
				reply(*it, msg);
			}
		}
	}
}

void Server::welcomeClient(Client &client)
{
	string	response;

	response  = ":ft_irc.1337.ma " + itos(RPL_WELCOME) + " :Welcome to the " + "ft_irc.1337.ma" + " Network, " + \
					client.getNick() + "!" + client.getUsername() + "@" + client.getIPAddr();
	reply(client, response);
	response = ":ft_irc.1337.ma " + itos(RPL_YOURHOST) + " :Your host is " + "ft_irc.1337.ma" + ", running version " + "version01";
	reply(client, response);
	response = ":ft_irc.1337.ma " + itos(RPL_CREATED) + " :This server was created " + getStartTime();
	reply(client, response);
}

void Server::changeNick(Client & client)
{
    string response;
    std::set<string> channelsJ;

    response = client.clientInfo() + " NICK :" + this->_params[1];
    channelsJ = client.getChannels();

    if (channelsJ.empty())
    {
        reply(client, response);
    }
    else
    {
        std::set<string>::iterator it = channelsJ.begin();
        for ( ; it != channelsJ.end(); it++)
        {
            channelIter itCha = doesChannelExist(*it);
            if (itCha != _channels.end())
            {
                std::set<string> users;
                users = itCha->getUserList();
                std::set<string>::iterator itUser = users.begin();
                for ( ; itUser != users.end(); itUser++)
                {
                    clientIter itClient = doesUserExit(*itUser);

                    if (itClient != _clients.end())
                    {
                        reply(*itClient, response);
                    }
                }
            }
            else 
            {
                continue;
            }
            itCha->swapUser(client.getNick(), this->_params[1]);
        } 
    }
}