#include "Server.hpp"

void Server::checkSpamClient(Client& client)
{
    clientIter it = _clients.begin();
    for ( ; it != _clients.end(); it++)
    {
        if (it->getNick() == client.getNick() && it->checkConnect() == false)
        {
            std::string response = "ERROR :Closing Link: " +  it->getNick() +  " by ft_irc.1337.ma (Overridden by other sign on)\r\n";
            send(it->getSockfd(), response.c_str(), response.length(), 0);
            it->closeSocket();
            _pollfds[getIndexOfClient(it) + 1].fd = -1;
        }
    }
}
bool Server::checkAlreadyNick(std::string &nick)
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

std::string trim_internal(const std::string &str)
{
    string result;
    bool prev_is_space = false;

    for (string::size_type i = 0; i < str.size(); i++)
    {
        char c = str[i];
        if (isspace(c))
        {
            if (!prev_is_space)
            {
                result += c;
            }
            prev_is_space = true;
        }
        else
        {
            result += c;
            prev_is_space = false;
        }
    }
    if (result[0] == ' ')
        result.erase(0, 1);
    if (result[result.size() - 1] == ' ')
        result.erase(result.size() - 1, 1);
    return result;
}


void  Server::parseCommand(string &command)
{
    if (command.find(" :") != string::npos)
    {
        command.erase(0, command.find_first_not_of(" "));
        if (command[0] == ':')
            command.insert(0, " ");
        string temp = command;
        command = command.substr(0, command.find(" :"));
        std::stringstream ss(command);
        string token;
        while (std::getline(ss, token, ' '))
        {
            this->serverParamiters.push_back(token);
        }
        temp = temp.substr(temp.find(":") + 1);
        this->serverParamiters.push_back(temp);
    }
    else
    {
        command = trim_internal(command);
        std::stringstream ss(command);
        string token;
        while (std::getline(ss, token, ' '))
        {
            this->serverParamiters.push_back(token);
        }
    }
}

void Server::handleCommand(string& cmd, int id)
{
    parseCommand(cmd);
    cmdmapIter it = this->commandMap.find(this->serverParamiters[0]);
    if (it != this->commandMap.end())
    {
        // if not connected check this for connection
        (this->*it->second)(_clients[id]);
    }
    else
    {
        cerr << "Error: invalid command" << endl;
    }
    this->serverParamiters.clear();
}


// bool Server::parseCommandClient(char *buffer, Client &client)
// {
//     bool isCommand = true;
//     int len = std::strlen(buffer) - 2;
//     if (buffer[len] == '\r')
//     {
//         buffer[len] = '\n';
//         buffer[len + 1] = '\0';
//     }
//     std::stringstream ss(buffer);
//     string token;
//     while (getline(ss, token))
//     {
//         if (ss.eof())
//         {
//             client.appendBuffer(token);
//             token.clear();
//             isCommand = false;
//         }
//         else 
//         {
//             // cout << "command: ||" << token << "||" << endl;
//             client.appendBuffer(token);
//             handleCommand(client.getBuffer());
//             token.clear();
//             client.getBuffer().clear();
//             isCommand = true;
//         }
//     }
//     return isCommand;
// }