#include "IRCbot.hpp"

IRCbot::Channel::Channel(const string &name) : _name(name)
{
}

// void IRCbot::Channel::debugInfo(void)	// only for debug
// {
// 	if (!_loggedUsers.empty())
// 	{
// 		std::cout << ">>>>>>>>>>>>> [ ALL USERS " << _name << " ] <<<<<<<<<<<<<<" << std::endl;
// 		for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
// 		{
// 			std::cout << it->_nick << std::endl;
// 		}
// 	}
// 	if (!_operators.empty())
// 	{
// 		std::cout << ">>>>>>>>>>>>> [ OPERATORS " << _name << " ] <<<<<<<<<<<<<<" << std::endl;
// 		for (strVecIter it = _operators.begin(); it < _operators.end(); it++)
// 		{
// 			std::cout << *it << " " << std::endl;
// 		}
// 	}
// 	if (!_badUsers.empty())
// 	{
// 		std::cout << ">>>>>>>>>>>>> [ BLACKLIST " << _name << " ] <<<<<<<<<<<<<<" << std::endl;
// 		for (strVecIter it = _badUsers.begin(); it < _badUsers.end(); it++)
// 		{
// 			std::cout << *it << " " << std::endl;
// 		}
// 	}
// 	std::cout << std::endl;
// }

void IRCbot::Channel::updateOperators(std::vector<string> &tokens)
{
	strVecIter it;

	if (tokens[3] == "+o")
	{
		_operators.push_back(tokens[4]);
	}
	if (tokens[3] == "-o")
	{
		if ( (it = std::find(_operators.begin(), _operators.end(), tokens[4])) != _operators.end())
		{
			_operators.erase(it);
		}
	}
}

void IRCbot::Channel::updateUsers(std::vector<string>& tokens, string& botnick)
{
	string user;

	user = getUserNick(tokens[0]);

	if (tokens[1] == "JOIN" && user != botnick)
	{
		User cli(user);
		_loggedUsers.push_back(cli);
	}
	else if (tokens[1] == "KICK")
	{
		userIter it =  doesUserExit(user);
		if (it != _loggedUsers.end())
		{
			removeUser(tokens[3]);
		}
	}
}

void IRCbot::Channel::logUsers(string &users)
{
	string				user;

	std::stringstream ss(users);
	while (ss >> user)
	{
		if (user[0] == '@')
		{
			_operators.push_back(user.substr(1));
			User cli(user.substr(1));
			_loggedUsers.push_back(cli);
		}
		else
		{
			User cli(user);
			_loggedUsers.push_back(cli);
		}
	}
}

void IRCbot::Channel::removeUser(const string &user)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == user)
		{
			_loggedUsers.erase(it);
			break;
		}
	}
	for (strVecIter it = _operators.begin(); it < _operators.end(); it++)
	{
		if (*it == user)
		{
			_operators.erase(it);
			break;
		}
	}
	for (strVecIter it = _badUsers.begin(); it < _badUsers.end(); it++)
	{
		if (*it == user)
		{
			_badUsers.erase(it);
			break;
		}
	}
}

void IRCbot::Channel::updateUserNick(const string &old_nick, const string &new_nick)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == old_nick)
		{
			it->_nick = new_nick;
			break;
		}
	}
	for (strVecIter it = _operators.begin(); it < _operators.end(); it++)
	{
		if (*it == old_nick)
		{
			*it = new_nick;
			break;
		}
	}
	for (strVecIter it = _badUsers.begin(); it < _badUsers.end(); it++)
	{
		if (*it == old_nick)
		{
			*it = new_nick;
			break;
		}
	}
}

long	IRCbot::Channel::getTime(const string& user)
{
	userIter it =  doesUserExit(user);

	if (it != _loggedUsers.end())
	{
		long res = getTimeInMinutes() - it->_timer;
		return (res);
	}
	return 0;
}


IRCbot::userIter IRCbot::Channel::doesUserExit(const string& user)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == user)
		{
			return (it);
		}
	}
	return (_loggedUsers.end());
}

bool IRCbot::Channel::isOperator(string &user)
{
	return (std::find(_operators.begin(), _operators.end(), user) != _operators.end());
}

bool IRCbot::Channel::isMember(string &user)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == user)
		{
			return (true);
		}
	}
	return (false);
}

string IRCbot::Channel::getBadUsers(void)
{
	string r;
	
	for (size_t i = 0; i < _badUsers.size(); i++)
	{
		if (i != 0)
			r += " ";
		if (isOperator(_badUsers[i]))
			r += "@";
		r += _badUsers[i];
	}
	return (r);
}

void IRCbot::Channel::addBadUser(string &user)
{
	static int i;
	std::vector<string>::iterator it = std::find(_badUsers.begin(), _badUsers.end(), user);
	if (it == _badUsers.end())
	{
		if (_badUsers.size() < 10)
			_badUsers.push_back(user);
		else
			_badUsers[i % 10] = user;
		i++;
	}
}
