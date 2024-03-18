#include "Channel.hpp"

void Channel::setCreationTime(void)
{
    std::time_t res = std::time(NULL);

    char *timePtr = std::ctime(&res);

    if (!timePtr)
    {
        cerr << "error time" << endl;
        _creationTime = "";
    }
    else
    {
        _creationTime = timePtr;
    }
}

Channel::Channel(const string &channelName, const string &admin)
{
    joinUser(admin);
    setChannelOperator(admin);
    _admin = admin;
    _name = channelName;
    _modes = "+t";
    _topic = "";
    _userLimit = -1;		// or string?
    _passkey = "";
    _hasInvite = false;
    _hasPasskey = false;
    _hasLimit = false;
    _hasTopic = false;
}

Channel::~Channel() {}

bool Channel::joinUser(const string& user)
{
    // if modes has "+l" >> if _users.size() + 1 > _userLimit > don't add
    if ((_users.insert(user)).second)
    {
        cout << user << " joined channel " << _name << endl;
        return (true);
    }
    cout << user << " already in channel " << _name << endl;
    return (false);
}

bool Channel::isUserInChannel(const string &user) const
{
    if (_users.find(user) != _users.end())
        return (true);
	return false;
}

bool Channel::isUserOperator(const string &user) const
{
    if (!isUserInChannel(user))
        return (false);
    if (_operators.find(user) != _operators.end())
        return (true);
    return (false);
}

bool Channel::unsetChannelOperator(const string &user)
{
    if (!isUserInChannel(user))
        return (false);
    _operators.erase(user);
    cout << user << " is no longer an operator in channel " << _name << endl;
    return (true);
}

bool Channel::partUser(const string& user)
{
    if (_users.erase(user))     // returns Number of elements removed (0 or 1)
    {
        cout << user << " left channel " << _name << endl;
        if (isUserOperator(user))
            _operators.erase(user);
        return (true);
    }
    cout << user << " is not in channel " << _name << endl;
    return (false);
}




std::set<string> Channel::getUserList() const
{
    return (_users);
}

std::set<string> Channel::getOperatorList() const
{
	return (_operators);
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

string Channel::channelModeIs() const
{
    string s = _modes;
    for (size_t i = 0; i < _modes.size(); i++)
    {
        if (_modes[i] == 'k')
        {
            s += " " + this->getPasskey();
        }
        if (_modes[i] == 'l')
        {
            s += " " + std::to_string(this->getUserLimit());
        }
    }
    return (s);
}

void Channel::printUsers()
{
    cout << "Users in channel " << _name << ": ";

    for (std::set<string>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}

void Channel::printOperators()
{
    cout << "Operators in channel " << _name << ": ";

    for (std::set<string>::iterator it =_operators.begin(); it !=_operators.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}

bool Channel::setChannelOperator(const string &user)
{
    if (!isUserInChannel(user))
        return (false);
    _operators.insert(user);
    cout << user << " is now an operator in channel " << _name << endl;
    return (true);
}

bool Channel::setMode(const string &mode)
{
    // if mode == "-o" > if param is in channel > add to operator list

    // this code is not tested !!!!
    if (mode[0] == '+')
    {
        if (_modes.find(mode[1]) == string::npos)
        {
            _modes += mode[1];
            return (true);
        }
    }
    else if (_modes.find(mode[1]) != string::npos)
    {
        _modes.erase(_modes.find(mode[1]), 1);
        return (true);
    }
    return (false);
}

void Channel::setTopic(const string &topic)
{
    _topic = topic;
}

void Channel::setPasskey(const string &key)
{
    _passkey = key;
    _hasPasskey = true;
}

void Channel::setUserLimit(string limit)
{
    // TODO: parse limit
    _userLimit = std::atoi(limit.c_str());
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
