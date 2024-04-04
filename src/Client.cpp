#include "Client.hpp"

Client::Client()
{
}

Client::Client(const string &ip, int port, int sockfd)
	: _ip(ip), _port(port), _clifd(sockfd)
{
	_nick = "*";
	_username = "*";
	_real = "";
	_hostname = "";
	_recvBuf = "";
	_hasPassed = false;
	_hasUsedNick = false;  
	_hasUsedUser = false;
}

Client::~Client()
{
}

int Client::getSockfd(void) const
{
	return (_clifd);
}

void Client::closeSocket()
{
	close(_clifd);
    this->_clifd = -1;
}

int Client::getPort(void) const
{
	return (_port);
}

string Client::getIPAddr(void) const
{
	return (_ip);
}

string& Client::rdBuf(void)
{
	return (_recvBuf);
}

std::queue<string>& Client::sdBuf(void)
{
	return (_sendBuf);
}

void Client::setHasPassed(bool value)
{
    this->_hasPassed = value;
}

void Client::setHasUsedNick(bool value)
{
    this->_hasUsedNick = value;
}

void Client::setHasUsedUser(bool value)
{
    this->_hasUsedUser = value;
}

void Client::inviteToChannel(const string& channelName)
{
    if (!isInvitedToChannel(channelName))
    {
        _invitedChannels.push_back(channelName);
    }
}

void Client::uninviteFromChannel(const string& chan)
{
    std::vector<string>::iterator it;

    it = std::find(_invitedChannels.begin(), _invitedChannels.end(), chan);
    if (it != _invitedChannels.end())
        _invitedChannels.erase(it);
}

bool Client::isInvitedToChannel(const string& chan) const
{
    std::vector<string>::const_iterator it;
    
    it = std::find(_invitedChannels.begin(), _invitedChannels.end(), chan);
    if (it != _invitedChannels.end())
    {
        return true;
    }
    return false;
}

std::vector<string> Client::getInvitedChannels(void) const
{
    return (_invitedChannels);
}

bool Client::getHasPassed()
{
    return this->_hasPassed;
}

bool Client::getHasUsedNick()
{
    return this->_hasUsedNick;
}

bool Client::getHasUsedUser()
{
    return this->_hasUsedUser;
}

bool Client::checkNick(string &nick)
{
    if (nick.empty())
    {
        return false;
    }
    if (!isalpha(nick[0]))
    {
        return false;
    }
    for (size_t i = 1; i < nick.size(); ++i)
    {
        char current = nick[i];
        if (!isalnum(current) && current != '_' && current != '-' && current != '[' && \
            current != ']' && current != '\\' && current != '`' && \
            current != '^' &&  current != '{' && current != '}')
        {
            return false;
        }
    }
    return true;
}

string Client::identifier(void)
{
	return (":" + getNick() + "!~" + getUsername() + "@" + getIPAddr());
}

std::set<string> Client::getChannels(void)
{
    return (_channels);
}

const string &Client::getNick(void) const
{
    return this->_nick;
}

const string& Client::getUsername(void) const
{
    return this->_username;
}

void Client::addChannels(string &channel)
{
    _channels.insert(channel);
}

void Client::setNick(string nick)
{
    this->_nick = nick;
}

void Client::setUsername(string username)
{
    this->_username = username;
}

bool Client::isConnected()
{
    return (_hasUsedNick && _hasUsedUser && _hasPassed);
}
