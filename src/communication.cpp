#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{
    string resp = msg + "\r\n";
    client.sdBuf().push(resp);
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

	response  = ":ft_irc.1337.ma " + itos(RPL_WELCOME) + " " + client.getNick() +  " :Welcome to the " + "ft_irc.1337.ma" + " Network, " + \
				client.getNick() + "!" + client.getUsername() + "@" + client.getIPAddr();
	reply(client, response);
	response = ":ft_irc.1337.ma " + itos(RPL_YOURHOST) + " " + client.getNick() + " :Your host is " + "ft_irc.1337.ma" + ", running version " + "version: 01";
	reply(client, response);
	response = ":ft_irc.1337.ma " + itos(RPL_CREATED) + " " + client.getNick() + " :This server was created " + getStartTime();
	reply(client, response);
}

void Server::changeNick(Client & client)
{
    string              response;
    std::set<string>    channelsJ;

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


// void    Server::replyNotConnected(Client &client)
// {
//     string response;

//     response = ":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + \
//         client.getNick()  + " :You have not registered";
//     reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + \
//         client.getNick()  + " :You have not registered");
// }

// bool Server::channelSendMsg(Client& client, string& ch)
// {
//     string response;
//     bool found;

//     found = false;
//     for (size_t k = 0; k < _channels.size(); k++)
//     {
//         if (ch == _channels[k].getName())
//         {
//             found = true;
//             response = ":"  + client.getNick() + "!~" + client.getUsername()  + \
//                 "@" + client.getIPAddr() + " PRIVMSG " + _channels[k].getName() + " :";
//             broadcastMsg(client, (response  + _messagClient), _channels[k]);
//             break ;
//         }
//     }
//     return found;
// }

// bool Server::nickSendMsg(Client& client, string& nick)
// {
//     string response;
//     bool found;

//     found = false;
//     for (size_t j = 0; j < _clients.size(); j++)
//     {
//         if (nick == _clients[j].getNick())
//         {
//             found = true;
//             response = client.identifier() + " PRIVMSG " + _clients[j].getNick() + " :" +  _messagClient;
//             reply(_clients[j], response);
//             break ;
//         }
//     }
//     return found;
// }