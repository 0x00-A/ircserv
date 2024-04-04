#include "IRCbot.hpp"

void IRCbot::sendReply( const string &reply)
{
	string response;

	response = reply + "\r\n";
	if (write(ircSock, response.data(), response.length()) <= 0)
	{
		std::perror("write");
		throw (string("The bot was unable to send a reply to the server"));
	}
}

void IRCbot::blacklistReply(string &nick, Channel& chan)
{
	string response;

	if (!chan.isMember(nick))
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " You're not on the channel";
	}
	if (!chan.isOperator(nick))
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " You're not channel operator";
	}
	else
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " :" + chan.getBadUsers();
	}
	sendReply(response);
}

void IRCbot::logtimeReply(string &nick, Channel& chan)
{
	string response;

	if (!chan.isMember(nick))
	{
		response = "PRIVMSG " + nick + " :You're not on the channel";
	}
	else
	{
		response = "PRIVMSG " + nick + " :Logtime for " + nick + " is: " + itos(chan.getTime(nick)) + " min";
	}
	sendReply(response);
}
