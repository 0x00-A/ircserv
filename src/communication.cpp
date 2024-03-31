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
            reply(sender, _servname + " " + itos(ERR_CANNOTSENDTOCHAN) + " " + \
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

	response  = _servname + " " + itos(RPL_WELCOME) + " " + client.getNick() +  \
    " :Welcome to the " + _servname + " Network, " + client.identifier();
	reply(client, response);
	response = _servname + " " + itos(RPL_YOURHOST) + " " + client.getNick() + \
    " :Your host is " + _servname + ", running version " + "version: 01";
	reply(client, response);
	response = _servname + " " + itos(RPL_CREATED) + " " + client.getNick() + \
    " :This server was created " + getStartTime();
	reply(client, response);
}

void Server::changeNick(Client & client)
{
    string              response;
    std::set<string>    channelsJ;
    std::set<string> allreadysend;

    response = client.identifier() + " NICK :" + this->_params[1];
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
                        if (allreadysend.count(itClient->getNick()) > 0)
                            continue ;
                        allreadysend.insert(itClient->getNick());
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
