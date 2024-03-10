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
	_isRegistered = false;
	_isOperator = false;
	_hasPassed = false;
	_hasUsedNick = false;  
	_hasUsedUser = false;
}

Client::~Client()
{
	// leave channels
	// close fd
}

int Client::getSockfd(void) const
{
	return (_clifd);
}

void Client::closeSocket() const
{
	close(_clifd);
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
        if (!isalnum(current) &&
            current != '-' && current != '[' && current != ']' &&
            current != '\\' && current != '`' && current != '^' &&
            current != '{' && current != '}')
        {
            return false;
        }
    }
    return true;
}

string &Client::getNick(void)
{
    return this->_nick;
}

string &Client::getUsername(void)
{
    return this->_username;
}

void Client::setNick(string nick)
{
    this->_nick = nick;
}

void Client::setUsername(string username)
{
    this->_username = username;
}

bool Client::checkConnect()
{
    return (_hasUsedNick && _hasUsedUser && _hasPassed);
}
