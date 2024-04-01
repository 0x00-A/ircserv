#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(const string &channelName, const string &admin)
{
    _admin = admin;
    _name = channelName;
    _modes = "+t";
    _topic = "";
    _userLimit = -1;
    _passkey = "";
    _hasInvite = false;
    _hasPasskey = false;
    _hasLimit = false;
    _hasTopic = true;

    setCreationTime();
    joinUser(admin);
    setChannelOperator(admin);
}

Channel::~Channel() 
{
}

std::set<string> Channel::getUserList() const
{
    return (_users);
}

string Channel::getPasskey(void) const
{
	return (_passkey);
}

size_t Channel::getUserLimit(void) const
{
	return (_userLimit);
}

string Channel::getTopic(void) const
{
	return (_topic);
}

string Channel::getCreationTime() const
{
    return (_creationTime);
}

bool Channel::empty(void) const
{
    return (_users.size() == 0);
}

bool Channel::hasPasskey(void) const
{
	return (_hasPasskey);
}

bool Channel::hasUserLimit(void) const
{
	return (_hasLimit);
}

bool Channel::hasInvite(void) const
{
	return (_hasInvite);
}

bool Channel::hasTopic(void) const
{
	return (_hasTopic);
}

void Channel::setHasPasskey(bool stat)
{
    _hasPasskey = stat;
}

void Channel::setHasUserLimit(bool stat)
{
    _hasLimit = stat;
}

void Channel::setHasInvite(bool stat)
{
    _hasInvite = stat;
}

void Channel::setHasTopic(bool stat)
{
    _hasTopic = stat;
}

bool Channel::hasMode(char mode) const
{
	return (_modes.find(mode) != string::npos);
}

void Channel::setUsers(std::set<string> &users)
{
    _users = users;
}

bool Channel::setChannelOperator(const string &user)
{
    if (!isUserInChannel(user))
        return (false);
    if (isUserOperator(user))
        return (false);
    _users.erase(user);
    _users.insert("@" + user);
    cout << user << " is now an operator in channel " << _name << endl;
    return (true);
}

bool Channel::setMode(const string &mode)
{
    if (mode[0] == '+')
    {
        if (_modes.find(mode[1]) == string::npos)
        {
            if (_modes.empty())
                _modes += "+";
            _modes += mode[1];
            return (true);
        }
    }
    else if (mode[0] == '-')
    {
        if (_modes.find(mode[1]) != string::npos)
        {
            _modes.erase(_modes.find(mode[1]), 1);
            if (_modes == "+")
                _modes = "";
            return (true);
        }
    }
    return (false);
}

void Channel::setTopic(const string &topic)
{
    _topic = topic;
}

void Channel::setPasskey(const string &key)
{
    stringstream    ss;
    string          word, res;

    ss << key;
    while (ss >> word)
    {
        res += word;
    }
    _passkey = res;
    _hasPasskey = true;
}

void Channel::setUserLimit(string limit)
{
    _userLimit = std::strtol(limit.c_str(), NULL, 10);
    _hasLimit = true;
}

void Channel::unsetTopic( void )
{
    _topic = "";
    _hasTopic = false;
}

string Channel::getAdmin() const
{
    return (_admin);
}

size_t Channel::getSize() const
{
    return (_users.size());
}

string Channel::getName() const
{
    return _name;
}

string Channel::getModes(void) const
{
	return (_modes);
}

void Channel::setCreationTime(void)
{
    std::time_t     res;
    stringstream    ss;

    res = std::time(NULL);
    ss << res;
    _creationTime = ss.str();
}
