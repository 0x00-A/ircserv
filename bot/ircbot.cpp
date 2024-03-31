#include "ircbot.hpp"

ircbot::User::User(const string& nick) : _nick(nick)
{
	_timer = getTimeInMinutes();
}

ircbot::userIter ircbot::doesUserExit(const string& user)
{
	// std::cout << "::::::::::::::::::::::::::::::::::" << std::endl;
	// for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	// {
	// 	std::cout << ":::: > " << it->_nick << " <" << std::endl;
	// }
	// std::cout << "::::::::::::::::::::::::::::::::::" << std::endl;
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == user)// || it->_nick == ("@" + user))
		{
			return (it);
		}
	}
	return (_loggedUsers.end());
}

bool ircbot::isOperator(string &user)
{
	// strVecIter it = std::find(_operators.begin(), _operators.end(), user);
	return (std::find(_operators.begin(), _operators.end(), user) != _operators.end());
}

bool ircbot::isMember(string &user)
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

string ircbot::getBadUsers(void)
{
	string r;
	
	for (size_t i = 0; i < _badUsers.size(); i++)
	{
		// if (_badUsers[i].empty()) continue;
		if (i != 0)
			r += " ";
		if (isOperator(_badUsers[i]))
			r += "@";
		r += _badUsers[i];
	}
	return (r);
}

void ircbot::addBadUser(string &user)
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
		std::cout << "added " << user << " to blacklist" << std::endl;
	}
}

void ircbot::sendReply(const string &reply)
{
	if (write(_botSock, reply.data(), reply.length()) <= 0)
	{
		std::perror("write");
		throw (string("The bot was unable to send a reply to the server"));
	}
}

bool ircbot::isErrorCode(const string &code)
{
	return ( code == ERR_NEEDMOREPARAMS || code == ERR_PASSWDMISMATCH || code == ERR_NOTREGISTERED
			|| code == ERR_ERRONEUSNICKNAME || code == ERR_NICKNAMEINUSE || code == ERR_NOSUCHCHANNEL
			|| code == ERR_INVITEONLYCHAN || code == ERR_BADCHANNELKEY || code == ERR_CHANNELISFULL);
}

void ircbot::blacklistReply(string &nick)
{
	string response;

	if (!isMember(nick))
	{
		response = "PRIVMSG " + nick + " :" + _channel + " You're not on the channel\n";
	}
	if (!isOperator(nick))
	{
		response = "PRIVMSG " + nick + " :" + _channel + " You're not channel operator\n";
	}
	else
	{
		response = "PRIVMSG " + nick + " :" + _channel + " :" + getBadUsers() + "\n";
	}
	sendReply(response);
}

void ircbot::logtimeReply(string &nick)
{
	string response;

	// TODO: handle users not in the channel !!
	if (!isMember(nick))
	{
		response = "PRIVMSG " + nick + " :You're not on the channel\n";
	}
	else
	{
		response = "PRIVMSG " + nick + " :Logtime for " + nick + " is: " + itos(getTime(nick)) + " min\n";
	}
	sendReply(response);
}

string ircbot::itos(int num)
{
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

ircbot::ircbot(string passwd, string port, string nick, string chan)
	: _passwd(passwd), _ircPort(port), _recvbuf(""), _nick(nick), _channel(chan)
{
	const int	enable = 1;

	if ( (_botSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		exit(1);
	}
	if (setsockopt(_botSock, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(int)))
	{
		perror("setsockopt");
	}
	
	// store data
	std::ifstream infile("offensive_words.txt");
	if (infile.is_open())
	{
		string s;
		while(std::getline(infile, s))
		{
			_wordlist.push_back(s);
		}
	}
}

long ircbot::getTimeInMinutes()
{
	std::time_t currentTime;
    std::time(&currentTime);

    return currentTime / 60;
}

void ircbot::debugInfo(void)
{
	if (!_loggedUsers.empty())
	{
		std::cout << ">>>>>>>>>>>>> [ ALL USERS ] <<<<<<<<<<<<<<" << std::endl;
		for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
		{
			std::cout << it->_nick << "(" << it->_timer << ")" << " | " << std::endl;
		}
		std::cout << "\n\n";
	}
	if (!_operators.empty())
	{
		std::cout << ">>>>>>>>>>>>> [ OPERATORS ] <<<<<<<<<<<<<<" << std::endl;
		for (strVecIter it = _operators.begin(); it < _operators.end(); it++)
		{
			std::cout << *it << " " << std::endl;
		}
		std::cout << "\n\n";
	}
	if (!_badUsers.empty())
	{
		std::cout << ">>>>>>>>>>>>> [ BLACKLIST ] <<<<<<<<<<<<<<" << std::endl;
		for (strVecIter it = _badUsers.begin(); it < _badUsers.end(); it++)
		{
			std::cout << *it << " " << std::endl;
		}
	}
	std::cout << std::endl;
}

long	ircbot::getTime(const string& user)
{
	userIter it =  doesUserExit(user);

	if (it != _loggedUsers.end())
	{
		long res = getTimeInMinutes() - it->_timer;
    	return (res);
	}
    return 0;
}

void	ircbot::connectToServer()
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(_ircPort.c_str()));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (connect(_botSock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		std::perror("connect");
		throw (string("The bot was unable to connect to server"));
	}
	std::cout << "Connected to server successfully" << std::endl;
}

void	ircbot::registerBot()
{
	std::vector<string> tokens;
	string				cmd;

	std::stringstream ss;
    // ss << "PASS " << _passwd << "\r\n";
    ss << "NICK " << _nick << "\r\n";
    // ss << "USER " << _nick << " 0 * :" << _nick << "\r\n";
    ss << "JOIN " << _channel << "\r\n";

	sendReply(ss.str());
}

void	ircbot::handleRead()
{
	ssize_t 			read_size;
	char				buffer[BUF_SIZE];


	read_size = read(_botSock, buffer, sizeof(buffer) - 1);
	if (read_size <= 0)
	{
		std::perror("read");
		throw (string("read failed!"));
	}
	buffer[read_size] = '\0';
	_recvbuf += buffer;
	// std::cout << "buf: " << _recvbuf << std::endl;
}

string	ircbot::getCommand()
{
	size_t pos;
	string cmd = "";

	if ( (pos = _recvbuf.find("\n")) != std::string::npos)
	{
		cmd = _recvbuf.substr(0, pos);
		// std::cout << "cmd: " << cmd << std::endl;

		_recvbuf = _recvbuf.substr(pos + 1);
	}
	return (cmd);
}

void ircbot::parseCommand(string &cmd, std::vector<string> &tokens)
{
    size_t pos;
    std::stringstream ss;
    string token, tmp = "";

	// if (tokens[3][tokens[3].size() - 1] == '\n')
	// 	tokens[3].erase(tokens[3].size() - 1, 1);
	// if (tokens[3][tokens[3].size() - 1] == '\r')
	// 	tokens[3].erase(tokens[3].size() - 1, 1);
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

void	ircbot::handleCommand(std::vector<string>& tokens)
{
	string				response, usersList, userNick, command;

	command = tokens[1];
	if (isErrorCode(command))
	{
		throw (tokens.back());
	}
	else if (command == RPL_NAMREPLY)
	{
		logUsers(tokens.back());
	}
	else if (command == "PRIVMSG")
	{
		userNick = getUserNick(tokens[0]);
		if (tokens[2] == _nick && tokens.back() == "logtime")
		{
			logtimeReply(userNick);
		}
		if (tokens[2] == _nick && tokens.back() == "blacklist")
		{	
			blacklistReply(userNick);
		}
		if (tokens[2] != _nick)		// not just a privmsg to bot
		{
			checkOffensiveWords(tokens);
		}
	}
	else if (command == "JOIN" || command == "KICK" || command == "QUIT" || command == "NICK")
	{
		// TODO: handle quit and NICK
		updateUsers(tokens);
		debugInfo();
	}
	else if (command == "MODE" && (tokens[3] == "+o" || tokens[3] == "-o"))
	{
		updateOperators(tokens);
		debugInfo();
	}
}

string ircbot::getUserNick(string &token)
{
	size_t pos = token.find_first_of('!');
	return (token.substr(1, pos - 1));
}

void ircbot::checkOffensiveWords(std::vector<string> &tokens)
{
	string userNick;

	userNick = getUserNick(tokens[0]);
	if (hasBadWords(tokens.back()))
	{
		sendReply("PRIVMSG " + userNick + " :Please refrain from using inappropriate language.\n");
		addBadUser(userNick);
	}
}

bool ircbot::hasBadWords(string &str)
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

void ircbot::updateOperators(std::vector<string> &tokens)
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

void ircbot::updateUsers(std::vector<string>& tokens)
{
	string user;

	user = getUserNick(tokens[0]);

	if (tokens[1] == "JOIN")
	{
		if (user == _nick) return ;
		User cli(user);
		_loggedUsers.push_back(cli);
	}
	else if (tokens[1] == "KICK" || tokens[1] == "QUIT")
	{
		// TODO: remove from _operatos
		// TODO: remove from _badUsers list
		userIter it =  doesUserExit(user);
		if (it != _loggedUsers.end())
		{
			removeUser(user);
		}
	}
	else if ((tokens[1] == "NICK"))
	{
		// TODO: change in _operatos
		// TODO: change in _badUsers
		// TODO: change in _loggedUsers
		userIter it =  doesUserExit(user);
		if (it != _loggedUsers.end())
		{
			updateUserNick(user, tokens.back());
		}
	}
}

void ircbot::logUsers(string &users)
{
	string				user;

	std::stringstream ss(users);
	while (ss >> user)
	{
		if (user == _nick || "@" + user == _nick) continue ;

		// TODO: handle "@" in nicknames of operatos and TEST !! 

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
		std::cout << "BOT INFO: user `" << user << "` logged" << std::endl;
	}
}

void ircbot::removeUser(const string &user)
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
	std::cout << "BOT INFO: removed user `" << user << "`" << std::endl;
}

void ircbot::updateUserNick(const string &old_nick, const string &new_nick)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == old_nick)
		{
			*it = new_nick;
			std::cout << "user `" << old_nick << "` changed nick to `" << new_nick << "`" << std::endl;
			break;
		}
	}
	for (strVecIter it = _operators.begin(); it < _operators.end(); it++)
	{
		if (*it == old_nick)
		{
			*it = new_nick;
			std::cout << "operator `" << old_nick << "` changed nick to `" << new_nick << "`" << std::endl;
			break;
		}
	}
	for (strVecIter it = _badUsers.begin(); it < _badUsers.end(); it++)
	{
		if (*it == old_nick)
		{
			*it = new_nick;
			std::cout << "blacklist nick  `" << old_nick << "` changed to `" << new_nick << "`" << std::endl;
			break;
		}
	}
	std::cout << "BOT INFO: user `" << old_nick << "` changed nick to `" << new_nick << "`" << std::endl;
}

void	ircbot::run()
{
	//
	std::vector<string> tokens;
	string				response, cmd;

	registerBot();
	while (true)
	{
		handleRead();
		
		while ( (cmd = getCommand()) != "")
		{
			// parse command
			parseCommand(cmd, tokens);
			// handle command
			handleCommand(tokens);
			tokens.clear();
		}
	}
}

ircbot::~ircbot()
{
	close(_botSock);
}
