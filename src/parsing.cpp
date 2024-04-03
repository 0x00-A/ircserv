#include "Server.hpp"

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

        throw ( _servname + " " + ERR_UNKNOWNCOMMAND + " " + \
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

string Server::getCommand(int id)
{
	string& rdBuf = _clients[id].rdBuf();
	size_t	pos;
	string	cmd = "";

	if (!rdBuf.empty() && ((pos = rdBuf.find("\n")) != string::npos))
	{
		cmd = rdBuf.substr(0, pos);
		if ( cmd[pos - 1] == '\r')
		{
			cmd[pos - 1] = '\n';
		}
		rdBuf = rdBuf.substr(pos + 1);
	}
	return (cmd);
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
