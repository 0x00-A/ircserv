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
}

void Server::handleCommand(string& cmd, int id)
{
    this->_messagClient.clear();
    this->_sendMsgClient.clear();
    this->_params.clear();
    _keys.clear();
    _parsChannels.clear();

    parseCommand(cmd);
    if (this->_params.empty()) return;

    cmdmapIter it = this->commandMap.find(this->_params[0]);
    if (it != this->commandMap.end())
    {
        (this->*it->second)(_clients[id]);
    }
    else if (_clients[id].isConnected())
    {
        throw ( ":ft_irc.1337.ma " + intToString(ERR_UNKNOWNCOMMAND) + " " + \
        _clients[id].getNick() + " " + _params[0]  + " :Unknown command" );
    }
    // this->_messagClient.clear();
    // this->_sendMsgClient.clear();
    // this->_params.clear();
}

string Server::trim_comma(const string &str)
{
    string result;
    bool prev_is_comma = false;

    for (size_t i = 0; i < str.size(); i++)
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

void Server::initPrivmsg(Client &client)
{
    string response;
    if (_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NORECIPIENT) + " " + \
            client.getNick()  + " ::No recipient given (" + _params[0] + ")";
        reply(client, response);
        return;
    }
    if (_params.size() < 3)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NOTEXTTOSEND) + " " + \
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
void Server::initJoin(Client &client)
{
    string line;
    string chan;
    string key = "";
    bool    keys = false;
    std::stringstream ssk;

    std::set<std::string> seenChannels;
    string response;
    if (_params.size() < 2)
    {
        response = ":ft_irc.1337.ma " + intToString(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick() + " JOIN" + " :Not enough parameters";
        reply(client, response);
        return;
    }
    line = trim_comma(_params[1]);
    std::stringstream ss(line);

    if (_params.size() > 2)
    {
        keys = true;
        line = trim_comma(_params[2]);
        ssk << line;
    }
    while (std::getline(ss, chan, ','))
    {
        key = "";
        if (keys)
            std::getline(ssk, key, ',');
        if (seenChannels.count(chan) > 0) 
        {
            response = ":ft_irc.1337.ma " + intToString(ERR_TOOMANYTARGETS) + \
             " " +  client.getNick() + " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenChannels.insert(chan);
        _parsChannels.push_back(std::make_pair(chan, key));
    }
}

void Server::parsepasswd(const string& passwd) const
{
    if (passwd.empty())
        throw (std::invalid_argument("Invalid password."));
    // TODO: check for spaces
}
