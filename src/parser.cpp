#include "Server.hpp"

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

void  Server::parseCommand(string &command)
{
    size_t pos;
    stringstream ss;
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
    size_t pos;
    this->_messagClient.clear();
    this->_sendMsgClient.clear();
    this->_params.clear();
    _keys.clear();
    _parsChannels.clear();

    if ((pos = cmd.find("\n")) != string::npos)
        cmd.erase(pos);
    parseCommand(cmd);
    if (this->_params.empty()) return;
    to_upper(this->_params[0]);
    if (this->_params[0] == "PONG") return ;
    cmdmapIter it = this->commandMap.find(this->_params[0]);
    if (it != this->commandMap.end())
    {
        cout << "cmd: " << cmd << endl;
        (this->*it->second)(_clients[id]);
    }
    else if (_clients[id].isConnected())
    {

        throw ( _servname + " " + itos(ERR_UNKNOWNCOMMAND) + " " + \
        _clients[id].getNick() + " " + _params[0]  + " :Unknown command" );
    }
}

string Server::trim_comma(const string &str, int flg)
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
    if (flg)
    {
        if (result[0] == ',')
        result.erase(0, 1);
        if (result[result.size() - 1] == ',')
        result.erase(result.size() - 1, 1);
    }
    return result;
}

void Server::initPrivmsg(Client &client)
{
    if (!client.isConnected())
    {
        throw (_servname + " " + itos(ERR_NOTREGISTERED) + " " + client.getNick()  + " :You have not registered");
    }
    if (_params.size() < 2)
    {
        throw (_servname + " " + itos(ERR_NORECIPIENT) + " " + client.getNick()  + " :No recipient given (" + _params[0] + ")");
    }
    if (_params.size() < 3)
    {
        throw (_servname + " " + itos(ERR_NOTEXTTOSEND) + " " + client.getNick()  + " ::No text to send");
    }
    string clients = trim_comma(_params[1], 1);
    stringstream ss(clients);
    string token;
    while (std::getline(ss, token, ','))
    {
        if (token[0] == '#')
            _sendMsgClient.push_back(std::make_pair(token, CHANNEL));
        else
            _sendMsgClient.push_back(std::make_pair(token, CLIENT));
    }
    _messagClient = _params[_params.size() - 1];
}

void Server::initJoin(Client &client)
{
    string              line, chan, key = "", response;
    stringstream        ssk;
    std::set<string>    seenChannels;
    bool                keys = false;

    if (_params.size() < 2)
    {
        throw (_servname + " " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + \
                " JOIN" + " :Not enough parameters");
    }
    line = trim_comma(_params[1], 1);
    stringstream ss(line);
    if (_params.size() > 2)
    {
        keys = true;
        line = trim_comma(_params[2], 0);
        cout << "linekey: " << line << endl;
        ssk << line;
    }
    while (std::getline(ss, chan, ','))
    {
        key = "";
        if (keys)
        {
            std::getline(ssk, key, ',');
            cout << " >> key " << key << endl;

        }
        if (seenChannels.count(chan) > 0) 
        {
            response = _servname + " " + itos(ERR_TOOMANYTARGETS) + " " +  client.getNick() + \
                    " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenChannels.insert(chan);
        _parsChannels.push_back(std::make_pair(chan, key));
    }
}

void Server::parseargs() const
{
    if (_passwd.empty())
        throw (std::invalid_argument("Invalid password"));
    if (_port.empty() || _port == "nan")
        throw (std::invalid_argument("Invalid port number"));
    char * endptr;
    double d = std::strtod(_port.c_str(), &endptr);
    if (*endptr)
        throw (std::invalid_argument("Invalid port number"));
    if (d <= 0 || d > 65535)
        throw (std::invalid_argument("Port range not valid"));
}
