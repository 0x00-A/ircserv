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

// 11.67