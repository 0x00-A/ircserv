#include "Server.hpp"

void Server::pass(Client &client)
{
    if (client.checkConnect())
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ALREADYREGISTRED) + " " + \
            client.getNick()  + " :You may not reregister";
        reply(client, response);
        return ;
    }
    if (this->serverParamiters.size() < 2)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
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
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_PASSWDMISMATCH) + " " + \
            client.getNick()  + " " + this->serverParamiters[1] + " :Password incorrect";
        reply(client, response);
    }
}

// bool Server::checkNickFormeClient(Client &client)
// {
//     clientIter it = std::find(_clients.begin(), _clients.end(), client);
//     --it;
//     if (it->getNick() == client.getNick())
//     {
//         return false;
//     }
//     return true;
// }
void Server::nick(Client &client)
{
    // i wnat check here
    if (client.getHasPassed() == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :dir b3da pass hhh";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 2)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NONICKNAMEGIVEN) + " " + \
            client.getNick()  + " :No nickname given";
        reply(client, response);
        return;
    }
    if (client.checkNick(this->serverParamiters[1]) == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :Erroneus nickname";
        reply(client, response);
        return;
    }
    if (checkAlreadyNick(this->serverParamiters[1]) == false)
    {
        // this command not work now you moust add how find all client for check nick (add clinets in vector);
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NICKNAMEINUSE) + " " + \
            client.getNick()  + " :Nickname is already in use";
        reply(client, response);

    }
    if (!client.getNick().empty())
    {
        if (client.getHasUsedNick() == true)
        {
            std::string response = ":" + client.getNick() + "!@ " + this->serverParamiters[0] + " :" +  this->serverParamiters[1];
            reply(client, response);
        }
        client.setNick(this->serverParamiters[1]);
        client.setHasUsedNick(true);
    }
}

void Server::user(Client &client)
{
    if (client.getHasPassed() == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :dir b3da pass";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 5)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->serverParamiters[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (!client.getUsername().empty())
    {
        client.setHasUsedUser(true);
        client.setUsername(this->serverParamiters[1]);
        std::cout << "size:: " << _clients.size() << std::endl;
        if (_clients.size() > 1)
        {
            clientIter it = getClientIterator(client);
            --it;
            if (it->getNick() == client.getNick())
            {
                std::cout << "nick::1 " << it->getNick() << std::endl;
                std::cout << "nick::2 " << client.getNick() << std::endl;
                std::string response = "ERROR :Closing Link: " +  it->getNick() +  " by ft_irc.1337.ma (Overridden by other sign on)\r\n";
                send(it->getSockfd(), response.c_str(), response.length(), 0);
                // disconnectClient(it->getIndex());

                it->closeSocket();
                _pollfds[getIndexOfClient(it) + 1].fd = -1;
            }
        }
    }
}
