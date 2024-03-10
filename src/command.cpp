#include "Server.hpp"

void Server::pass(Client &client)
{
    if (client.checkConnect())
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_ALREADYREGISTRED) + " " + \
            client.getNick()  + " :You may not reregister";
        reply(client, response);
        return ;
    }
    if (this->serverParamiters.size() < 2)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->serverParamiters[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (this->serverParamiters[1] == this->_passwd)
    {
        client.setHasPassed(true);
    }
    else
    {
        // ERR_PASSWDMISMATCH
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_PASSWDMISMATCH) + " " + \
            client.getNick()  + " " + this->serverParamiters[1] + " :Password incorrect";
        reply(client, response);
    }
}

void Server::nick(Client &client)
{
    // i wnat check here
    if (client.getHasPassed() == false)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 2)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NONICKNAMEGIVEN) + " " + \
            client.getNick()  + " :No nickname given";
        reply(client, response);
        return;
    }
    if (client.checkNick(this->serverParamiters[1]) == false)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :Erroneus nickname";
        reply(client, response);
        return;
    }
    if (checkAlreadyNick(this->serverParamiters[1]) == false)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NICKNAMEINUSE) + " " + \
            client.getNick()  + " :Nickname is already in use";
        reply(client, response);

    }
    if (!client.getNick().empty())
    {
        if (client.getHasUsedNick() == true)
        {
            string response = ":" + client.getNick() + "!@ " + this->serverParamiters[0] + " :" +  this->serverParamiters[1];
            reply(client, response);
        }
        client.setNick(this->serverParamiters[1]);
        client.setHasUsedNick(true);
        if (_clients.size() > 1 && client.getHasUsedUser())
        {
            checkSpamClient(client);
        }
    }
}


void Server::user(Client &client)
{
    if (client.getHasPassed() == false)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NOTREGISTERED) + " " + \
            client.getNick()  + " :You have not registered";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 5)
    {
        string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->serverParamiters[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (!client.getUsername().empty())
    {
        client.setHasUsedUser(true);
        client.setUsername(this->serverParamiters[1]);

        if (_clients.size() > 1 && client.getHasUsedNick())
        {
            cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
            checkSpamClient(client);
        }
    }
}

void Server::quit(Client &client)
{
    string response = "ERROR:: by by\r\n";
    send(client.getSockfd(), response.c_str(), response.length(), 0);
    client.closeSocket();
    _pollfds[getIndexOfClient(client) + 1].fd = -1;
}

void Server::join(Client &client)
{
    (void)client;
}
