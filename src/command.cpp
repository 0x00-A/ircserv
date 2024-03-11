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
    (void)client;
    string clients = trim_comma(serverParamiters[1]);
    cout << "++++++++++++++++++++++++++++++++" << endl;
    cout << "nick Clients: |" <<  clients << "|" << endl;
    cout << "++++++++++++++++++++++++++++++++" << endl;


    std::stringstream ss(clients);
    string token;
    while (std::getline(ss, token, ','))
    {
        // _sendMsgClient.push_back(token);
        if (token.front() == '#')
            _sendMsgClient.push_back(std::make_pair(token, CHANNEL));
        else
            _sendMsgClient.push_back(std::make_pair(token, CLIENT));

    }

    for (size_t i = 0; i < _sendMsgClient.size(); i++)
    {
        cout << "nickClient[" << i << "]: " << _sendMsgClient[i].first << ">>" << _sendMsgClient[i].second << endl;
    }
    _messagClient = serverParamiters[serverParamiters.size() - 1];
    cout << "messgClient: " << _messagClient << endl;
}
