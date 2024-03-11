#include "Server.hpp"

void Server::checkSpamClient(Client& client)
{
    clientIter it = _clients.begin();
    for ( ; it != _clients.end(); it++)
    {
        if (it->getNick() == client.getNick() && it->checkConnect() == false)
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
        if (it->checkConnect())
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
        this->serverParamiters.push_back(token);
    }
    if (!tmp.empty())
        this->serverParamiters.push_back(tmp);

    // for (size_t i = 0; i < serverParamiters.size(); i++)
    // {
    //     cout << "p |" << serverParamiters[i] << "|" << endl;
    // }
}

void Server::handleCommand(string& cmd, int id)
{
    parseCommand(cmd);
    if (this->serverParamiters.empty()) return;
    cmdmapIter it = this->commandMap.find(this->serverParamiters[0]);
    if (it != this->commandMap.end())
    {
        (this->*it->second)(_clients[id]);
    }
    else
    {
        cerr << "Error: invalid command" << endl;
    }
    this->_messagClient.clear();
    this->_sendMsgClient.clear();
    this->serverParamiters.clear();
}

void Server::initPrivmsg()
{
    string clients = trim_comma(serverParamiters[1]);
    std::stringstream ss(clients);
    string token;
    while (std::getline(ss, token, ','))
    {
        if (token.front() == '#')
            _sendMsgClient.push_back(std::make_pair(token, CHANNEL));
        else
            _sendMsgClient.push_back(std::make_pair(token, CLIENT));

    }
    _messagClient = serverParamiters[serverParamiters.size() - 1];
}
