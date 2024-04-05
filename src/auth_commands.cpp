#include "Server.hpp"
#include "Channel.hpp"

void Server::pass(Client &client)
{
    if (client.isConnected())
    {
        throw (_servname + " " + ERR_ALREADYREGISTRED + " " + client.getNick()  + " :You may not reregister");
    }
    if (this->_params.size() < 2)
    {
        throw (_servname + " " + ERR_NEEDMOREPARAMS + " " + client.getNick()  + " " + this->_params[0] + \
                " :Not enough parameters");
    }
    if (this->_params[1] == this->_passwd)
    {
        client.setHasPassed(true);
    }
    else
    {
        client.setHasPassed(false);
        throw (_servname + " " + ERR_PASSWDMISMATCH + " " + client.getNick()  + " " + this->_params[1] + \
                " :Password incorrect");
    }
}

void Server::nick(Client &client)
{
    bool    welcome = false;

    if (client.getHasPassed() == false)
    {
        throw (_servname + " " + ERR_NOTREGISTERED + " " + client.getNick()  + " :You have not registered");
    }
    if (this->_params.size() < 2)
    {
        client.setHasUsedNick(false);
        throw (_servname + " " + ERR_NONICKNAMEGIVEN + " " + client.getNick()  + " :No nickname given");
    }
    if (this->_params[1].size() >= NICKLEN)
    {
        this->_params[1].erase(NICKLEN);
    }
    if (client.checkNick(this->_params[1]) == false)
    {
        client.setHasUsedNick(false);
        throw (_servname + " " + ERR_ERRONEUSNICKNAME + " " + client.getNick()  + " :Erroneus nickname");
    }
    if (checkAlreadyNick(this->_params[1]) == false)
    {
        throw (_servname + " " + ERR_NICKNAMEINUSE + " " + client.getNick()  + " :Nickname is already in use");
    }
    if (client.getHasUsedNick() == true && !client.isConnected())
    {
        reply(client, client.getNick() + "!@ NICK :" +  this->_params[1]);
    }
    if (client.isConnected())
    {
        changeNick(client);
        welcome = true;
    }
    client.setNick(this->_params[1]);
    client.setHasUsedNick(true);
    if (client.isConnected() && !welcome)
    {
        welcomeClient(client);
    }
    if (_clients.size() > 1 && client.getHasUsedUser())
    {
        checkSpamClient(client);
    }
}


void Server::user(Client &client)
{
    if (client.isConnected())
    {
        throw (_servname + " " + ERR_ALREADYREGISTRED + " " + client.getNick()  + " :You may not reregister");
    }
    if (client.getHasPassed() == false)
    {
        throw (_servname + " " + ERR_NOTREGISTERED + " " + client.getNick()  + " :You have not registered");
    }
    if (this->_params.size() < 5)
    {
        throw (_servname + " " + ERR_NEEDMOREPARAMS + " " + client.getNick()  + " " + this->_params[0] + \
                " :Not enough parameters");
    }
    if (this->_params[1].size() >= USERNAMELEN)
        this->_params[1].erase(USERNAMELEN);
    client.setUsername(this->_params[1]);
    client.setHasUsedUser(true);
    if (_clients.size() > 1 && client.getHasUsedNick())
    {
        checkSpamClient(client);
    }
    if (client.isConnected())  welcomeClient(client);
}

/********************************************************************************/
/*                                     UTILS                                    */
/********************************************************************************/

void Server::checkSpamClient(Client& client)
{
    clientIter it = _clients.begin();
    for ( ; it != _clients.end(); it++)
    {
        if (it->getNick() == client.getNick() && it->isConnected() == false)
        {
            string response = "ERROR :Closing Link: " +  it->getNick() +  " by " + _servname + \
                                " (Overridden by other sign on)\r\n";
            send(it->getSockfd(), response.c_str(), response.length(), 0);
            it->closeSocket();
            _pollfds[getIndexOfClient(it) + 1].fd = -1;
        }
    }
}
bool Server::checkAlreadyNick(string &nick)
{
    std::vector<Client>::iterator it = _clients.begin();
    for (; it != _clients.end(); it++)
    {
        if (it->isConnected())
        {
            if (it->getNick() == nick)
                return false;
        }
    }
    return true;
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
