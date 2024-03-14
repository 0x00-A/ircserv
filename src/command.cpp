#include "Server.hpp"

void Server::pass(Client &client)
{
    string response;

    if (client.isConnected())
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_ALREADYREGISTRED) + " " + \
            client.getNick()  + " :You may not reregister";
        reply(client, response);
        return ;
    }
    if (this->_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NEEDMOREPARAMS) + " " + \
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
        response = ":ft_irc.1337.ma " + to_string(RPL_WELCOME) + " " + \
            client.getNick()  + " " + this->_params[1] + " :Password incorrect";
        reply(client, response);
    }
}

void Server::nick(Client &client)
{
    string response;

    if (client.getHasPassed() == false)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NONICKNAMEGIVEN) + " " + \
            client.getNick()  + " :No nickname given";
        reply(client, response);
        return;
    }
    if (client.checkNick(this->_params[1]) == false)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :Erroneus nickname";
        reply(client, response);
        return;
    }
    if (checkAlreadyNick(this->_params[1]) == false)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NICKNAMEINUSE) + " " + \
            client.getNick()  + " :Nickname is already in use";
        reply(client, response);

    }
    if (client.getHasUsedNick() == true)
    {
        response = ":" + client.getNick() + "!@ " + this->_params[0] + " :" +  this->_params[1];
        reply(client, response);
    }
    client.setNick(this->_params[1]);
    client.setHasUsedNick(true);
    if (_clients.size() > 1 && client.getHasUsedUser())
    {
        checkSpamClient(client);
    }
    if (client.isConnected())
    {
        response = ":ft_irc.1337.ma " + to_string(RPL_WELCOME) + " " + \
            client.getNick()  + " :Welcome to the 1337 IRC Network " + client.getNick();
        reply(client, response);
    }
}


void Server::user(Client &client)
{
    string response;

    if (client.getHasPassed() == false)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->_params.size() < 5)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NEEDMOREPARAMS) + " " + \
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
        response = ":ft_irc.1337.ma " + to_string(ERR_NICKNAMEINUSE) + " " + \
            client.getNick()  + " :Welcome to the 1337 IRC Network " + client.getNick();
        reply(client, response);
    }
}

void Server::quit(Client &client)
{
    // :dan-!d@localhost QUIT :Quit: Bye for now!
    string response = ":" + client.getNick() + "-!" + client.getUsername() + \
        "@" + client.getIPAddr() + " QUIT :Quit: Bye for now!\r\n";
    send(client.getSockfd(), response.c_str(), response.length(), 0);
    client.closeSocket();
    _pollfds[getIndexOfClient(client) + 1].fd = -1;
}

void Server::join(Client &client)
{
    (void)client;
}

string Server::trim_comma(const string &str)
{
    string result;
    bool prev_is_comma = false;

    for (string::size_type i = 0; i < str.size(); i++)
    {
        char c = str[i];
        if (c == ',')
        {
            if (!prev_is_comma)
            {
                result += c;
            }
            prev_is_comma = true;
        }
        else
        {
            result += c;
            prev_is_comma = false;
        }
    }
    if (result[0] == ',')
        result.erase(0, 1);
    if (result[result.size() - 1] == ',')
        result.erase(result.size() - 1, 1);
    return result;
}


void Server::privmsg(Client &client)
{
    // ERR_CANNOTSENDTOCHAN >> if don't find client in channel  
    // ERR_NOTOPLEVEL 

    std::set<std::string> seenNicks;
    bool found = false;
    string response;
    if (!client.isConnected())
    {
        //  :stockholm.se.quakenet.org 451 *  :You have not registered
        response = ":ft_irc.1337.ma " + to_string(ERR_NOTREGISTERED) + " " + \
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
            response = ":ft_irc.1337.ma " + to_string(ERR_TOOMANYTARGETS) + \
             " " +  client.getNick() + " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenNicks.insert(_sendMsgClient[i].first);
        for (size_t j = 0; j < _clients.size(); j++)
        {
            if (_sendMsgClient[i].first == _clients[j].getNick())
            {
                found = true;
                response = ":"  + client.getNick() + "!~" + client.getUsername()  + "@" + \
                 client.getIPAddr() + " PRIVMSG " + _clients[j].getNick() + " :" +  _messagClient;
                reply(_clients[j], response);
                break ;
            }
        }
        if (!found && (_sendMsgClient[i].second == CLIENT))
        {
            response = ":ft_irc.1337.ma " + to_string(ERR_NOSUCHNICK) + \
            " " +  client.getNick() + " " + _sendMsgClient[i].first + " :No such nick";
            reply(client, response);
        }
        if (!found && (_sendMsgClient[i].second == CHANNEL))
        {
            response = ":ft_irc.1337.ma " + to_string(ERR_NOSUCHNICK) + \
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
            if ( (c == 'o' || c == 'k' || (c == 'l' && sign == "+")) && !(k < _params.size()))
            {
                throw ( ":ft_irc.1337.ma " + to_string(ERR_NEEDMOREPARAMS) + " " + \
                client.getNick() + " " + _params[0]  + " :Not enough parameters" );
            }
            if (c == 'o' || c == 'k' || c == 'l')
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), _params[k++]));
            else
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), ""));
        }
        else
        {
            throw ( ":ft_irc.1337.ma " + to_string(ERR_UNKNOWNMODE) + " " + client.getNick() + \
            " " + _params[2].substr(i, 1) + " :is an unknown mode char to me" );
        }
    }
    return (true);
}

/* ERRORS
//      >> mode #chan1 +ik
//         +i+k   +ik
*/

void    Server::mode(Client& client)
{
    std::queue<std::pair<string, string> >    modes;
    channelIter         chan;
    string              paramsave;
    string              modesave;

    _channels.push_back(Channel("#c"));
    _channels.begin()->joinUser("aaa");
    _channels.begin()->setUserAsOperator("aaa");

    if (_params.size() < 2)
    {
        throw (":ft_irc.1337.ma " + to_string(ERR_NEEDMOREPARAMS) + " " + \
        client.getNick() + " " + _params[0]  + " :Not enough parameters");
    }
    if ( (chan = doesChannelExist(_params[1])) == _channels.end())
    {
        throw (":ft_irc.1337.ma " + to_string(ERR_NOSUCHCHANNEL) + " " + \
        client.getNick() + " " + _params[1]  + " :No such channel");
    }
    if (_params.size() == 2)
    {
        throw (":ft_irc.1337.ma " + to_string(RPL_CHANNELMODEIS) + " " + \
        client.getNick() + " " + _params[1] + " " + chan->channelMmodeIs());
    }
    parseModes(modes, client);
    if (!chan->isUserOperator(client.getNick()))
    {
        throw (":ft_irc.1337.ma " + to_string(ERR_CHANOPRIVSNEEDED) + " " + \
        client.getNick() + " " + _params[1] + " " + " :You're not channel operator");
    }
    while (!modes.empty())
    {
        std::pair<string, string> m = modes.front();

        if (m.first == "+o" || m.first == "-o")
        {
            if (doesUserExit(m.second) == _clients.end())
            {
                // mode #ch1 +o ddd // does't exist
                // :silver.libera.chat 401 lalala ddd :No such nick/channel
                throw ("");
            }
            if (!chan->isUserInChannel(m.second))
            {
                // mode #ch1 +o dddd   // is not in channel
                // :silver.libera.chat 441 lalala dddd #ch1 :They aren't on that channel
                throw ("");
            }
            (m.first == "+o")? chan->setChannelOperator(m.second) : chan->unsetChannelOperator(m.second);
        }
        else if (m.first == "+i")
        {
            chan->setInviteOnly(true);
        }
        else if (m.first == "-i")
        {}
        else if (m.first == "+k")
        {}
        else if (m.first == "-k")
        {}
        else if (m.first == "+l")
        {}
        else if (m.first == "-l")
        {}
        else if (m.first == "+t")
        {}
        else if (m.first == "-t")
        {}

        switch (m.first[1])
        {
            case 'o':
                if (!chan->setUserAsOperator(m.second))
                {
                    throw (":ft_irc.1337.ma " + to_string(401) + " " + \
                    client.getNick() + " " + _params[1] + " " + " :You're not channel operator");
                }
                break;
            case 'i':
                chan->setInviteOnly();
                break;
            case 'k':
                if (m.first[0] == '+')
                {
                    chan->setPasskey(m.second);
                    chan->setHasPasskey(true);
                }
                if (m.first[0] == '-' && chan->hasPasskey())
                {
                    if (chan->getPasskey() == m.second)
                    {
                        chan->setPasskey("*");
                        chan->setHasPasskey(false);
                    }
                }
                break;
            case 't':
                chan->setTopic(m.second);
                chan->setHasTopic();
                break;
            case 'l':
                if (!chan->setUserLimit(m.second))
                {
                    modes.pop();
                    continue;
                }
                chan->setHasInvite();
                break;
            default:
                break;
        }

        if (chan->setMode(m.first) || m.first[1] == 'k')
        {
            modesave += m.first;
            // if (!empty)
            paramsave += " " + m.second;
        }
        modes.pop();
    }
    reply(client, " " + modesave + " " + paramsave);
}

// TODO: problen in -k to change pass