#include "IRCbot.hpp"

string	IRCbot::getCommand()
{
	size_t pos;
	string cmd = "";

	if ( (pos = _recvbuf.find("\n")) != std::string::npos)
	{
		cmd = _recvbuf.substr(0, pos);
		_recvbuf = _recvbuf.substr(pos + 1);
	}
	return (cmd);
}

void IRCbot::parseCommand(string &cmd, std::vector<string> &tokens)
{
	size_t				pos;
	std::stringstream	ss;
	string 				token, tmp = "";

	if (cmd[cmd.size() - 1] == '\n')
	{
		cmd.erase(cmd.size() - 1, 1);
	}
	if (cmd[cmd.size() - 1] == '\r')
	{
		cmd.erase(cmd.size() - 1, 1);
	}
	if ( (pos = cmd.find(" :")) != string::npos)
	{
		tmp = cmd.substr(pos + 2);
		cmd = cmd.substr(0, pos);
	}
	ss << cmd;
	while (ss >> token)
	{
		tokens.push_back(token);
	}
	if (!tmp.empty())
		tokens.push_back(tmp);
}

std::string IRCbot::parseInfo(std::string marker, string endMarker, std::string& response)
{
    size_t MarkerPos = response.find(marker);
    if (MarkerPos != std::string::npos)
	{
        size_t start = MarkerPos + marker.size();
        size_t end = response.find(endMarker, start);
        std::string info = response.substr(start, end - start);
        return (info);
    }
	else
	{
        return ("not found");
    }
}

std::pair<string, string> IRCbot::parseRequest(string &token)
{
	std::pair<string, string>	p;
	size_t 						end = 0;


	size_t pos = token.find_first_not_of(" ");
	if ((pos != string::npos) && (token[pos] == '#'))
	{
		end = token.find_first_of(" ", pos);
		p.first = token.substr(pos, end);
	}
	else
	{
		p.first = "";
	}
	pos = token.find_first_not_of(" ", end + 1);
	if (pos != string::npos)
	{
		p.second = token.substr(pos);
	}
	else
	{
		p.second = "";
	}
	return (p);
}

void IRCbot::checkOffensiveWords(std::vector<string> &tokens)
{
	string userNick;
	chanIt it;

	userNick = getUserNick(tokens[0]);
	it = getChanIt(tokens[2]);
	if (it != _channels.end() && hasBadWords(tokens.back()))
	{
		sendReply("PRIVMSG " + it->_name + " :Please refrain from using inappropriate language, " + userNick + ".");
		it->addBadUser(userNick);
	}
}

bool IRCbot::hasBadWords(string &str)
{
	std::stringstream ss(str);
	string	token;

	while (ss >> token)
	{
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);
		if (std::find(_wordlist.begin(), _wordlist.end(), token) != _wordlist.end())
			return (true);
	}
	return (false);
}

string IRCbot::getUserNick(string &token)
{
	size_t pos = token.find_first_of('!');
	return (token.substr(1, pos - 1));
}
