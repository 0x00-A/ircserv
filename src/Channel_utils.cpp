#include "Channel.hpp"

bool Channel::isUserInChannel(const string &user) const
{
    return (_users.find(user) != _users.end() || _users.find("@" + user) != _users.end() );
}

bool Channel::isUserOperator(const string &user) const
{
    return (_users.find("@" + user) != _users.end());
}

bool Channel::unsetChannelOperator(const string &user)
{
    if (!isUserInChannel(user) || !isUserOperator(user))
        return (false);
    _users.erase("@" + user);
    _users.insert(user);
    return (true);
}

bool Channel::partUser(const string& user)
{
    if (_users.erase(user) || _users.erase("@" + user))
    {
        return (true);
    }
    return (false);
}

void Channel::swapUser(const string &oldUser, const string &newUser)
{
    std::set<string>::iterator itUser = _users.begin();

    for ( ; itUser != _users.end(); itUser++)
    {
        string  oldAdmin = ("@" + oldUser);
        string  newAdmin = ("@" + newUser);

        if (*itUser ==  oldAdmin)
        {
            _users.erase(oldAdmin);
            _users.insert(newAdmin);
            break;
        }
        else if (*itUser == oldUser)
        {
            _users.erase(oldUser);
            _users.insert(newUser);
            break;
        }
    }
}

bool Channel::joinUser(const string& user)
{
    if ((_users.insert(user)).second)
    {
        return (true);
    }
    return (false);
}


string Channel::channelModeIs() const
{
    stringstream s;
    s << _modes;
    for (size_t i = 0; i < _modes.size(); i++)
    {
        if (_modes[i] == 'k')
        {
            s << (" " + this->getPasskey());
        }
        if (_modes[i] == 'l')
        {
            s << " " ;
            s << this->getUserLimit();
        }
    }
    return (s.str());
}
