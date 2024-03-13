#include "Channel.hpp"

Channel::Channel(const string& channelName) : _name(channelName) {}

Channel::~Channel() {}

bool Channel::joinUser(const string& user)
{
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

bool Channel::setUserAsOperator(const string &user)
{
    if (!isUserInChannel(user))
        return (false);
    _operators.insert(user);
    return (true);
}

bool Channel::partUser(const string& user)
{
    if (_users.erase(user))     // returns Number of elements removed (0 or 1)
    {
        cout << user << " left channel " << _name << endl;
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

// void Channel::broadcastMessage(Client &sender, string message)
// {
//     cout << "[" << name << "] " << sender.getNick() << ": " << message << endl;
// }

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

string Channel::getName() const
{
    return _name;
}
