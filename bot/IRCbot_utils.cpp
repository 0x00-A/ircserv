#include "IRCbot.hpp"

IRCbot::User::User(const string& nick) : _nick(nick)
{
	_timer = getTimeInMinutes();
}

bool IRCbot::isErrorCode(const string &code)
{
	return ( code == ERR_NEEDMOREPARAMS || code == ERR_PASSWDMISMATCH || code == ERR_NOTREGISTERED
			|| code == ERR_ERRONEUSNICKNAME || code == ERR_NICKNAMEINUSE || code == ERR_NOSUCHCHANNEL
			|| code == ERR_INVITEONLYCHAN || code == ERR_BADCHANNELKEY || code == ERR_CHANNELISFULL);
}

long IRCbot::getTimeInMinutes()
{
	std::time_t currentTime;
	std::time(&currentTime);

	return currentTime / 60;
}

string IRCbot::itos(int num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

