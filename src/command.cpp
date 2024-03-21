#include "Server.hpp"

void Server::pass(Client &client)
{
    string response;

    std::cout << "receive pass command" << std::endl;
    if (client.isConnected())
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_ALREADYREGISTRED) + " " + \
            client.getNick()  + " :You may not reregister";
        reply(client, response);
        return ;
    }
    if (this->_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->_params[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (this->_params[1] == this->_passwd)
    {
        client.setHasPassed(true);
    }
    else
    {
        response = ":ft_irc.1337.ma " + intToString(RPL_WELCOME) + " " + \
            client.getNick()  + " " + this->_params[1] + " :Password incorrect";
        reply(client, response);
    }
}

void Server::nick(Client &client)
{
    string response;
    std::set<string> channelsJ;

    std::cout << "receive nick command\n";
    if (client.getHasPassed() == false)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NOTREGISTERED) + " " + client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NONICKNAMEGIVEN) + " " + client.getNick()  + " :No nickname given";
        reply(client, response);
        return;
    }
    if (client.checkNick(this->_params[1]) == false)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_ERRONEUSNICKNAME) + " " + client.getNick()  + " :Erroneus nickname";
        reply(client, response);
        return;
    }
    if (checkAlreadyNick(this->_params[1]) == false)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NICKNAMEINUSE) + " " + client.getNick()  + " :Nickname is already in use";
        reply(client, response);
        return;
    }
    if (client.getHasUsedNick() == true && !client.isConnected())
    {
        response = ":" + client.getNick() + "!~"  + client.getUsername()  + "@" + client.getIPAddr() + " NICK :" +  this->_params[1];
        reply(client, response);
    }
    client.setNick(this->_params[1]);
    client.setHasUsedNick(true);
    if (client.isConnected())
    {
        response = client.clientInfo() + " NICK :" + client.getNick();
        reply(client, response);

        channelsJ = client.getChannels();
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
                    /// 
                }
                

            }
        }
        
    }
    if (_clients.size() > 1 && client.getHasUsedUser())
    {
        checkSpamClient(client);
    }
    if (client.isConnected())
    {
        response = ":ft_irc.1337.ma 001 " + \
            client.getNick()  + " :Welcome to the 1337 IRC Network " + client.getNick();
        std::cout << response << '\n';
        reply(client, response);
    }
}


void Server::user(Client &client)
{
    string response;

    std::cout << "receive user command\n";
    if (client.getHasPassed() == false)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->_params.size() < 5)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->_params[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    client.setHasUsedUser(true);
    client.setUsername(this->_params[1]);
    if (_clients.size() > 1 && client.getHasUsedNick())
    {
        checkSpamClient(client);
    }
    if (client.isConnected())
    {
        response = ":ft_irc.1337.ma 001 " + \
            client.getNick()  + " :Welcome to the 1337 IRC Network " + client.getNick();
        std::cout << response << '\n';
        reply(client, response);
    }
}

void Server::quit(Client &client)
{
    string response = client.identifier() + " QUIT :Quit: Bye for now!\r\n";
    send(client.getSockfd(), response.c_str(), response.length(), 0);
    client.closeSocket();
    _pollfds[getIndexOfClient(client) + 1].fd = -1;
}

void Server::join(Client &client)
{
    string response;

    if (!client.isConnected())
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    initJoin(client);
    for (size_t i = 0; i < _parsChannels.size(); i++)
    {
        if (_parsChannels[i].first[0] != '#')
        {
            response = ":ft_irc.1337.ma " + intToString(ERR_NOSUCHCHANNEL) + \
            " " +  client.getNick() + " " + _parsChannels[i].first + " :No such channel";
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
    std::set<string> seenNicks;
    bool found = false;
    string response;


    if (!client.isConnected())
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    initPrivmsg(client);
    for (size_t i = 0; i < _sendMsgClient.size(); i++)
    {
        found = false;
        if (seenNicks.count(_sendMsgClient[i].first) > 0) 
        {
            response = ":ft_irc.1337.ma " + intToString(ERR_TOOMANYTARGETS) + \
             " " +  client.getNick() + " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenNicks.insert(_sendMsgClient[i].first);
        if (_sendMsgClient[i].first[0] == '#')
        {
            for (size_t k = 0; k < _channels.size(); k++)
            {
                if (_sendMsgClient[i].first == _channels[k].getName())
                {
                    found = true;
                    broadcastMsg(client, _messagClient, _channels[k]);
                    break ;
                }
            }
        }
        else
        {
            for (size_t j = 0; j < _clients.size(); j++)
            {
                if (_sendMsgClient[i].first == _clients[j].getNick())
                {
                    found = true;
                    response = client.identifier() + " PRIVMSG " + _clients[j].getNick() + " :" +  _messagClient;
                    reply(_clients[j], response);
                    break ;
                }
            }
        }
        if (!found && (_sendMsgClient[i].second == CLIENT))
        {
            response = ":ft_irc.1337.ma " + intToString(ERR_NOSUCHNICK) + \
            " " +  client.getNick() + " " + _sendMsgClient[i].first + " :No such nick/channel";
            reply(client, response);
        }
        if (!found && (_sendMsgClient[i].second == CHANNEL))
        {
            response = ":ft_irc.1337.ma " + intToString(ERR_NOSUCHCHANNEL) + \
            " " +  client.getNick() + " " + _sendMsgClient[i].first + " :No such channel";
            reply(client, response);
        }
    }   
}

/********************************************************************************/
/* The MODE command is provided so that channel operators may change the        */
/* characteristics of `their' channel.                                          */
/********************************************************************************/

// MODES

//	o - give/take channel operator privileges;
//	i - invite-only channel flag;
//	t - topic settable by channel operator only flag;
//	l - set the user limit to channel;
//	k - set a channel key (password).

// Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]

// NOTES
//	When using the 'o' options, a restriction on a total of three per mode command has been imposed

/* Examples
    MODE #Finnish +i               ; Makes #Finnish channel 'invite-only'.
    MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on channel #Finnish.
    MODE #42 +k oulu                ; Set the channel key to "oulu".
    MODE #eu-opers +l 10            ; Set the limit for the number of users on channel to 10.
*/
/********************************************

 * if the channel already has the mode nothing happens

MODE
:zinc.libera.chat 461 rimi MODE :Not enough parameters
MODE eer
:zinc.libera.chat 403 rimi eer :No such channel
MODE #gggg +l/+o
:zinc.libera.chat 461 rimi MODE :Not enough parameters

MODE #gggg df
:zinc.libera.chat 472 rimi d :is an unknown mode char to me

MODE #gggg l key
MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnst

MODE #gggg +o fff
:zinc.libera.chat 401 rimi fff :No such nick/channel

MODE #gggg +l 123
:rimi!~x@197.230.30.146 MODE #gggg +l 123

MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnstl 123
:zinc.libera.chat 329 rimi #gggg 1710302483

MODE #gggg -t
:rimi!~x@197.230.30.146 MODE #gggg -t

MODE #gggg -l
:rimi!~x@197.230.30.146 MODE #gggg -l

// unessecery params are ignored
MODE #gggg +l 123 ddfdfffd
:rimi!~x@197.230.30.146 MODE #gggg +l 123
MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnstl 123
:zinc.libera.chat 329 rimi #gggg 1710302483


mode #ch1 --t+++i
:kkk!~x@197.230.30.146 MODE #ch1 -t+i

********************************************/

bool    Server::parseModes(std::queue< std::pair<string, string> >& modes, Client& client)
{
    string sign = "+";
    for (size_t i = 0, k = 3; i < _params[2].size(); i++)
    {
        if (_params[2][i] == '-' || _params[2][i] == '+')
        {
            sign = _params[2].substr(i, 1);
            continue;
        }
        char c = _params[2][i];
        if (c == 'o' || c == 'l' || c == 'k' || c == 't' || c == 'i')
        {
            if ( (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+")) && !(k < _params.size()))
            {
                throw ( ":ft_irc.1337.ma " + intToString(ERR_NEEDMOREPARAMS) + " " + \
                client.getNick() + " " + _params[0]  + " :Not enough parametersss" );
            }
            if (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+"))
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), _params[k++]));
            else
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), ""));
        }
        else
        {
            throw ( ":ft_irc.1337.ma " + intToString(ERR_UNKNOWNMODE) + " " + client.getNick() + \
            " " + _params[2].substr(i, 1) + " :is an unknown mode char to me" );
        }
    }
    return (true);
}

void Server::handleOperatorFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan, Client& cli)
{
    if (doesUserExit(m.second) == _clients.end())
    {
        reply(cli, ":ft_irc.1337.ma " + to_string(ERR_NOSUCHNICK) + " " + \
            cli.getNick() + " " + m.second + " :No such nick");
    }
    else if (!chan->isUserInChannel(m.second))
    {
        reply(cli, ":ft_irc.1337.ma " + to_string(ERR_USERNOTINCHANNEL) + " " + \
            cli.getNick() + " " + _params[1] + " :They aren't on that channel");
    }
    else
    {
        (m.first == "+o")? chan->setChannelOperator(m.second) : chan->unsetChannelOperator(m.second);
        modesave += m.first;
        paramsave += " " + m.second;
    }
}

void Server::handleLimitFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan)
{
    if (m.first == "+l")
    {
        if (std::atoi(m.second.c_str()) > 0)
        {
            chan->setMode(m.first);
            chan->setHasUserLimit(true);
            chan->setUserLimit(m.second);
            modesave += m.first;
            paramsave += " " + m.second;
        }
    }
    else if (m.first == "-l")
    {
        if (chan->setMode(m.first))
        {
            chan->setHasUserLimit(false);
            modesave += m.first;
        }
    }
}

void Server::handlePasskeyFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan)
{
    if (m.first == "+k")
    {
        chan->setMode(m.first);
        chan->setPasskey(m.second);
        modesave += m.first;
        paramsave += " " + m.second;
    }
    else if (m.first == "-k")
    {
        if (chan->hasPasskey())
        {
            chan->setMode(m.first);
            chan->setHasPasskey(false);
            modesave += m.first;
            paramsave += " *";
        }
    }
}

void Server::handleInviteFlag(strPair &m, string &modesave, channelIter &chan)
{
    if (chan->setMode(m.first))
    {
        (m.first == "+i")? chan->setHasInvite(true) : chan->setHasInvite(false);
        modesave += m.first;
    }
}

void Server::handleTopicFlag(strPair &m, string &modesave, channelIter &chan)
{
    if (chan->setMode(m.first))
    {
        (m.first == "+t") ? chan->setHasTopic(true) : chan->setHasTopic(true);
        modesave += m.first;
    }
}

void Server::removeExtraPlusMinus(string &s)
{
    bool sawPlus = false;
    bool sawMinus = false;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '+' && !sawPlus)
        {
            sawPlus = true;
            sawMinus = false;
        }
        else if (s[i] == '+' && sawPlus)
        {
            s.erase(i--, 1);
        }
        if (s[i] == '-' && !sawMinus)
        {
            sawMinus = true;
            sawPlus = false;
        }
        else if (s[i] == '-' && sawMinus)
        {
            s.erase(i--, 1);
        }
    }
}

void    Server::mode(Client& client)
{
    std::queue<std::pair<string, string> >      modes;
    channelIter                                 chan;
    string                                      paramsave;
    string                                      modesave;

    // test with nick(aaa) channel(c)
    _channels.push_back(Channel("c", "aaa"));
    // _channels.begin()->joinUser("aaa");
    // _channels.begin()->setChannelOperator("aaa");

    if (!client.isConnected())
    {
        throw (":ft_irc.1337.ma " + intToString(ERR_NOTREGISTERED) + " " + \
        client.getNick()  + " :You have not registered");
    }
    if (_params.size() < 2)
    {
        throw (":ft_irc.1337.ma " + intToString(ERR_NEEDMOREPARAMS) + " " + \
        client.getNick() + " " + _params[0]  + " :Not enough parameters");
    }
    if ( (chan = doesChannelExist(_params[1])) == _channels.end())
    {
        throw (":ft_irc.1337.ma " + intToString(ERR_NOSUCHCHANNEL) + " " + \
        client.getNick() + " " + _params[1]  + " :No such channel");
    }
    if (_params.size() == 2)
    {
        reply(client, (":ft_irc.1337.ma " + to_string(RPL_CHANNELMODEIS) + " " + \
        client.getNick() + " " + _params[1] + " " + chan->channelModeIs()));
        throw (":ft_irc.1337.ma " + to_string(RPL_CREATIONTIME) + " " + \
        client.getNick() + " " + _params[1] + " " + chan->getCreationTime());
    }
    parseModes(modes, client);
    if (!chan->isUserOperator(client.getNick()))
    {
        throw (":ft_irc.1337.ma " + intToString(ERR_CHANOPRIVSNEEDED) + " " + \
        client.getNick() + " " + _params[1] + " " + " :You're not channel operator");
    }
    while (!modes.empty())
    {
        std::pair<string, string> m = modes.front();
        switch (m.first[1])
        {
            case ('o'):
            handleOperatorFlag(m, modesave, paramsave, chan, client);
            break;
            case ('l'):
            handleLimitFlag(m, modesave, paramsave, chan);
            break;
            case ('k'):
            handlePasskeyFlag(m, modesave, paramsave, chan);
            break;
            case ('i'):
            handleInviteFlag(m, modesave, chan);
            break;
            case ('t'):
            handleTopicFlag(m, modesave, chan);
            break;
        }
        modes.pop();
    }
    removeExtraPlusMinus(modesave);
    if (!modesave.empty())
        throw (client.identifier() + " " + _params[0] + " " + _params[1] + " " + modesave + paramsave);
}
