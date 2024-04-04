#include "Server.hpp"
#include "Channel.hpp"

void Server::quit(Client &client)
{
    string              response;
    string              reason;

    reason = "Client Quit";
    if (_params.size() > 1)
    {
        reason  = _params[1];
    }
    response = client.identifier() + " QUIT :" + reason;
    broadcastToJoinedChannels(client, response);
    response += "\r\nERROR :Closing Link: " + client.getIPAddr()  + " (" + reason  + ")\r\n";
    write(client.getSockfd(), response.c_str(), response.length());
    client.closeSocket();
    _pollfds[getIndexOfClient(client) + 1].fd = -1;
}

void Server::join(Client &client)
{
    string  response;

    if (!client.isConnected())
    {
        throw (_servname + " " + ERR_NOTREGISTERED + " " + client.getNick()  + \
                " :You have not registered");
    }
    initJoin(client);
    for (size_t i = 0; i < _parsChannels.size(); i++)
    {
        if (_parsChannels[i].first[0] != '#')
        {
            response = _servname + " " + ERR_NOSUCHCHANNEL + " " + client.getNick() + " " + _parsChannels[i].first + \
                    " :No such channel";
            reply(client, response);
        }
        else
        {
            joinChannel(client, _parsChannels[i]);
        }
    }

}


void Server::privmsg(Client &client)
{
    std::set<string>    seenNicks;
    string              response;
    channelIter         chanIt;
    clientIter          cliIt;

    initPrivmsg(client);
    for (size_t i = 0; i < _sendMsgClient.size(); i++)
    {
        if (seenNicks.count(_sendMsgClient[i].first) > 0) 
        {
            response = _servname + " " + ERR_TOOMANYTARGETS + " " +  client.getNick() + \
                        " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenNicks.insert(_sendMsgClient[i].first);
        if (_sendMsgClient[i].first[0] == '#')
        {
            if ( (chanIt = doesChannelExist(_sendMsgClient[i].first)) != _channels.end())
            {
                response = client.identifier() + " PRIVMSG " + chanIt->getName() + " :" + _messagClient;
                broadcastMsg(client, response, *chanIt);
            }
            else
            {
                response = _servname + " " + ERR_NOSUCHCHANNEL + " " +  client.getNick() + " " + \
                            _sendMsgClient[i].first + " :No such channel";
                reply(client, response);
            }
        }
        else
        {
            if ( (cliIt = doesUserExit(_sendMsgClient[i].first)) != _clients.end())
            {
                response = client.identifier() + " PRIVMSG " + cliIt->getNick() + " :" +  _messagClient;
                reply(*cliIt, response);
            }
            else
            {
                response = _servname + " " + ERR_NOSUCHNICK + " " +  client.getNick() + " " + \
                                _sendMsgClient[i].first + " :No such nick/channel";
                reply(client, response);
            }
        }
    }   
}

/********************************************************************************/
/*                                     UTILS                                    */
/********************************************************************************/

void Server::initPrivmsg(Client &client)
{
    if (!client.isConnected())
    {
        throw (_servname + " " + ERR_NOTREGISTERED + " " + client.getNick()  + " :You have not registered");
    }
    if (_params.size() < 2)
    {
        throw (_servname + " " + ERR_NORECIPIENT + " " + client.getNick()  + " :No recipient given (" + _params[0] + ")");
    }
    if (_params.size() < 3)
    {
        throw (_servname + " " + ERR_NOTEXTTOSEND + " " + client.getNick()  + " ::No text to send");
    }
    string clients = trim_comma(_params[1], 1);
    stringstream ss(clients);
    string token;
    while (std::getline(ss, token, ','))
    {
        if (token[0] == '#')
            _sendMsgClient.push_back(std::make_pair(token, CHANNEL));
        else
            _sendMsgClient.push_back(std::make_pair(token, CLIENT));
    }
    _messagClient = _params[2];
}

void Server::initJoin(Client &client)
{
    string              line, chan, key = "", response;
    stringstream        ssk;
    std::set<string>    seenChannels;
    bool                keys = false;

    if (_params.size() < 2)
    {
        throw (_servname + " " + ERR_NEEDMOREPARAMS + " " + client.getNick() + \
                " JOIN" + " :Not enough parameters");
    }
    line = trim_comma(_params[1], 1);
    stringstream ss(line);
    if (_params.size() > 2)
    {
        keys = true;
        ssk << _params[2];
    }
    while (std::getline(ss, chan, ','))
    {
        key = "";
        if (keys)
        {
            std::getline(ssk, key, ',');
            cout << " >> key " << key << endl;

        }
        if (seenChannels.count(chan) > 0) 
        {
            response =_servname + " " + ERR_TOOMANYTARGETS + " " +  client.getNick() + \
                    " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        if (chan.size() >= CHANNELLEN)
        {
            chan.erase(CHANNELLEN);
        }
        seenChannels.insert(chan);
        _parsChannels.push_back(std::make_pair(chan, key));
    }
}