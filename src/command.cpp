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

********************************************/

bool    Server::ValidMode(char& c)
{
    return (c == 'o' || c == 'l' || c == 'k' || c == 't' || c == 'i');
}

// bool    Server::getModes(std::queue<char>& modes)
// {
//     if (_params[1].front() != '+' || _params[1].front() != '-')
//     {
//         if (!ValidMode(_params[1].front()))
//         {
//             // MODE #gggg df
//             // :zinc.libera.chat 472 rimi d :is an unknown mode char to me
//             return (false);
//         }
//         else
//         {
//             // MODE #gggg l key
//             // MODE #gggg
//             // :zinc.libera.chat 324 rimi #gggg +Cnst
//             return (false);
//         }
//     }
//     else if (_params[1].size() == 1)
//         return (false);
//     for (size_t i = 1; i < _params[1].size(); i++)
//     {
//         if (ValidMode(_params[1][i]))
//     }
    
// }

void    Server::mode(Client& client)
{
    (void)client;
    std::queue<char> modes;
    // add function hasChannel() to Server
    if (_params.size() < 1)
    {
        // not enough params
    }
    if (_params.size() == 1)
    {
        // if channel exist print channel modes
            // :zinc.libera.chat 324 rimi #gggg +Cnsl 123
            // :zinc.libera.chat 329 rimi #gggg 1710302483
        // else
            // :zinc.libera.chat 403 rimi eer :No such channel
    }
    // if (!getModes(modes))
    //     return ;
}
