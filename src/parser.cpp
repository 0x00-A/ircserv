#include "Server.hpp"

void Server::checkSpamClient(Client& client)
{
    clientIter it = _clients.begin();
    for ( ; it != _clients.end(); it++)
    {
        if (it->getNick() == client.getNick() && it->isConnected() == false)
        {
            string response = "ERROR :Closing Link: " +  it->getNick() +  " by ft_irc.1337.ma (Overridden by other sign on)\r\n";
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

void  Server::parseCommand(string &command)
{
    size_t pos;
    std::stringstream ss;
    string token, tmp = "";

    if ( (pos = command.find(" :")) != string::npos)
    {
        tmp = command.substr(pos + 2);
        command = command.substr(0, pos);
    }
    ss << command;
    while (ss >> token)
    {
        this->_params.push_back(token);
    }
    if (!tmp.empty())
        this->_params.push_back(tmp);

    // for (size_t i = 0; i < _params.size(); i++)
    // {
    //     cout << "p |" << _params[i] << "|" << endl;
    // }
}

void Server::handleCommand(string& cmd, int id)
{
    parseCommand(cmd);
    if (this->_params.empty()) return;
    cmdmapIter it = this->commandMap.find(this->_params[0]);
    if (it != this->commandMap.end())
    {
        (this->*it->second)(_clients[id]);
    }
    else if (_clients[id].isConnected())
    {
        string response = ":ft_irc.1337.ma " + to_string(ERR_UNKNOWNCOMMAND) + " " + \
        _clients[id].getNick() + " " + _params[0]  + " :Unknown command";
        reply(_clients[id], response);
    }
    this->_messagClient.clear();
    this->_sendMsgClient.clear();
    this->_params.clear();
}

void Server::initPrivmsg(Client &client)
{
    string response;
    if (_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NORECIPIENT) + " " + \
            client.getNick()  + " ::No recipient given (" + _params[0] + ")";
        reply(client, response);
        return;
    }
    if (_params.size() < 3)
    {
        response = ":ft_irc.1337.ma " + to_string(ERR_NOTEXTTOSEND) + " " + \
            client.getNick()  + " ::No text to send";
        reply(client, response);
        return;
    }
    string clients = trim_comma(_params[1]);
    std::stringstream ss(clients);
    string token;
    while (std::getline(ss, token, ','))
    {
        if (token.front() == '#')
            _sendMsgClient.push_back(std::make_pair(token, CHANNEL));
        else
            _sendMsgClient.push_back(std::make_pair(token, CLIENT));

    }
    _messagClient = _params[_params.size() - 1];
}
