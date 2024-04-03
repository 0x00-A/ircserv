#include "ircbot.hpp"

int IsBotRunning = true;

ircbot::User::User(const string& nick) : _nick(nick)
{
	_timer = getTimeInMinutes();
}

ircbot::userIter ircbot::Channel::doesUserExit(const string& user)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == user)// || it->_nick == ("@" + user))
		{
			return (it);
		}
	}
	return (_loggedUsers.end());
}

bool ircbot::Channel::isOperator(string &user)
{
	return (std::find(_operators.begin(), _operators.end(), user) != _operators.end());
}

bool ircbot::Channel::isMember(string &user)
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

string ircbot::Channel::getBadUsers(void)
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

void ircbot::Channel::addBadUser(string &user)
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
	if (write(_ircSock, reply.data(), reply.length()) <= 0)
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

void ircbot::blacklistReply(string &nick, Channel& chan)
{
	string response;

	if (!chan.isMember(nick))
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " You're not on the channel\n";
	}
	if (!chan.isOperator(nick))
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " You're not channel operator\n";
	}
	else
	{
		response = "PRIVMSG " + nick + " :" + chan._name + " :" + chan.getBadUsers() + "\n";
	}
	sendReply(response);
}

void ircbot::logtimeReply(string &nick, Channel& chan)
{
	string response;

	if (!chan.isMember(nick))
	{
		response = "PRIVMSG " + nick + " :You're not on the channel\n";
	}
	else
	{
		response = "PRIVMSG " + nick + " :Logtime for " + nick + " is: " + itos(chan.getTime(nick)) + " min\n";
	}
	sendReply(response);
}

string ircbot::itos(int num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

ircbot::ircbot(string passwd, string port, string nick)
	: _passwd(passwd), _ircPort(port), _recvbuf(""), _nick(nick)
{
	// const int	enable = 1;

	if ( (_ircSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		exit(1);
	}

	_weatherServIP = "37.139.20.5";
	// if (setsockopt(_ircSock, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(int)))
	// {
	// 	perror("setsockopt");
	// }
	signal(SIGPIPE, SIG_IGN);
	
	// store data
	std::ifstream infile("./data/offensive_words.txt");
	if (infile.is_open())
	{
		string s;
		while(std::getline(infile, s))
		{
			_wordlist.push_back(s);
		}
	}
	else
	{
		std::cerr << "could't open file needed by bot\n";
	}
}

long ircbot::getTimeInMinutes()
{
	std::time_t currentTime;
	std::time(&currentTime);

	return currentTime / 60;
}

void ircbot::Channel::debugInfo(void)	// only for debug
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

long	ircbot::Channel::getTime(const string& user)
{
	userIter it =  doesUserExit(user);

	if (it != _loggedUsers.end())
	{
		long res = getTimeInMinutes() - it->_timer;
		return (res);
	}
	return 0;
}

void	ircbot::connectToIRCServer()
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(_ircPort.c_str()));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// TODO ip addr of irc serv 

	if (connect(_ircSock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		std::perror("connect");
		throw (string("The bot was unable to connect to IRC server"));
	}
	std::cout << "Connected to server successfully" << std::endl;
}

void ircbot::connectToWeatherServer(void)
{
	struct sockaddr_in serv_addr;

	if ( (_weatherSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		close(_ircSock);
		exit(1);
	}

	uint32_t ipInt = inet_addr(_weatherServIP.c_str());
	if (ipInt == INADDR_NONE)
	{
		std::perror("inet_addr");
		close(_weatherSock);
		throw (string("Invalid IP address for weather server"));
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	serv_addr.sin_addr.s_addr = ipInt;

	if (connect(_weatherSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::perror("connect");
		close(_weatherSock);
		throw (string("Connection to weather server Failed"));
	}
}

void	ircbot::IRCServRegister()
{
	std::vector<string> tokens;
	string				cmd;

	std::stringstream ss;
	// ss << "PASS " << _passwd << "\r\n";
	ss << "NICK " << _nick << "\r\n";
	// ss << "USER " << _nick << " 0 * :" << _nick << "\r\n";
	// ss << "JOIN " << _channel << "\r\n";
	sendReply(ss.str());
}

string ircbot::getWeatherInfo()
{
    char buffer[1024] = {0};

    std::string request = 
        "GET /data/2.5/weather?q=Casablanca,MA,uk&APPID=851bb946327ee4bdc5230fe57cd6439f HTTP/1.1\r\n"
        "Host: 37.139.20.5\r\n"
        "Connection: close\r\n\r\n";

    write(_weatherSock, request.c_str(), request.size());
    read(_weatherSock, buffer, sizeof(buffer)-1);

    // std::string response = buffer;
	// std::cout << "buf: " << buffer << std::endl;
	return (buffer);
}

void	ircbot::handleRead()
{
	ssize_t 			read_size;
	char				buffer[BUF_SIZE];


	read_size = read(_ircSock, buffer, sizeof(buffer) - 1);
	if (read_size <= 0)
	{
		std::perror("read");
		throw (string("read failed!"));
	}
	buffer[read_size] = '\0';
	_recvbuf += buffer;
}

string	ircbot::getCommand()
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

void ircbot::parseCommand(string &cmd, std::vector<string> &tokens)
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

std::string ircbot::parseInfo(std::string marker, string endMarker, std::string& response)
{
    size_t MarkerPos = response.find(marker);
    if (MarkerPos != std::string::npos)
	{
        size_t start = MarkerPos + marker.size();
        size_t end = response.find(endMarker, start);
        std::string info = response.substr(start, end - start);
        // std::cout << "info: " << info << std::endl;
        return (info);
    }
	else
	{
        // std::cout << "info not found in response" << std::endl;
        return ("no info found");
    }
}

void ircbot::sendWeatherInfo(string& client_nick)
{
	connectToWeatherServer();

	string	response = getWeatherInfo();

	std::cout << "res: " << response << std::endl;
	// if (response.empty()){}

	std::string res, reply;

    std::cout << std::endl;
    
    reply = "Weather for " + parseInfo("\"name\":\"", "\"", response) + ", " + \
			parseInfo("\"country\":\"", "\"", response) + ":\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    
    reply = "- Coordinates: Longitude " + parseInfo("\"lon\":", "\"", response) + \
			" Latitude " + parseInfo("\"lat\":", "}", response) + "\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);

    reply = "- Weather Main: " + parseInfo("\"main\":\"", "\"", response) + "\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);

    reply = "- Weather Description: " + parseInfo("\"description\":\"", "\"", response) + "\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    
    // res = parseInfo("\"temp\":", ",", response);
	double d = std::strtod(parseInfo("\"temp\":", ",", response).c_str(), NULL);
	d -= 273.15;
	std::stringstream ss;
	ss << d;
    reply = "- Temperature: " + ss.str() + " Celsius\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);

    reply = "- Humidity: " + parseInfo("\"humidity\":", "}", response) + "%\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);

    reply = "- Wind Speed: " + parseInfo("\"speed\":", ",", response) + " m/s\n";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
	close(_weatherSock);
}

void	ircbot::handleCommand(std::vector<string>& tokens)
{
	string				response, usersList, userNick, command;
	chanIt				it;

	command = tokens[1];
	std::cout << "token cmd: " << command << std::endl;
	if (isErrorCode(command))
	{
		throw (tokens.back());
	}
	else if (command == "INVITE")
	{
		// :user5!~x@127.0.0.1 INVITE bot :#c
		std::cout << "bot invited to channel " << tokens.back() << std::endl;
		sendReply("JOIN " + tokens.back() + "\r\n");
		_channels.push_back(Channel(tokens.back()));
	}
	else if (command == RPL_NAMREPLY)
	{
		// :ft_irc.1337.ma 353 bot @ #c :bot @user5
		std::cout << "NAMEREPLY\n";
		it = getChanIt(tokens[4]);
		if (it != _channels.end())
		{
			std::cout << ">> logging users |" << tokens.back() << std::endl;
			it->logUsers(tokens.back());
		}
	}
	else if (command == "PRIVMSG")
	{
		// :user5!~x@127.0.0.1 PRIVMSG bot :hi
		userNick = getUserNick(tokens[0]);
		std::pair<string, string> pair = parseRequest(tokens.back());

		std::cout << "msg : " << tokens.back() << " channel : " << pair.first << " request : " << pair.second << std::endl;

		it = getChanIt(pair.first);

		if (tokens[2] == _nick && tokens.back() == "weather")		// not just a privmsg to bot
		{
			sendWeatherInfo(userNick);
		}
		else if (tokens[2] != _nick)		// not just a privmsg to bot
		{
			checkOffensiveWords(tokens);
		}
		else if (tokens[2] == _nick && it == _channels.end())
		{
			sendReply("PRIVMSG " + userNick + " :Usage #channel <logtime/blacklist>\n");
			// return;
		}
		else if (tokens[2] == _nick && pair.second == "logtime")
		{
			logtimeReply(userNick, *it);
		}
		else if (tokens[2] == _nick && pair.second == "blacklist")
		{	
			blacklistReply(userNick, *it);
		}
	}
	else if (command == "JOIN" || command == "KICK" || command == "QUIT" || command == "NICK")
	{
		// :u5!~x@127.0.0.1 NICK :u55
		// :user6!~x@127.0.0.1 QUIT :Client Quit
		if (command == "KICK" || command == "JOIN")
		{
			// :user4!~x@127.0.0.1 KICK #c user5 :user5
			// :user6!~x@127.0.0.1 JOIN #c
			chanIt it = getChanIt(tokens[2]);
			if (tokens[3] == _nick)
				updateChannels(it);
			else

		}
		chanIt it = getChanIt(tokens[2]);
		if (it != _channels.end())
		{
			if (tokens[3] == _nick)
				updateChannels(it);
			else
			{
				it->updateUsers(tokens);
				it->debugInfo();
			}
		}
	}
	else if (command == "MODE" && (tokens[3] == "+o" || tokens[3] == "-o"))
	{
		chanIt it = getChanIt(tokens[2]);
		if (it != _channels.end())
		{
			it->updateOperators(tokens);
			it->debugInfo();
		}
	}
}

void ircbot::updateChannels(chanIt &iter)
{
	_channels.erase(iter);
}

ircbot::chanIt ircbot::getChanIt(string &name)
{
	for (chanIt it = _channels.begin(); it < _channels.end(); it++)
	{
		if (it->_name == name)
			return (it);
	}
	return (_channels.end());
}

string ircbot::getUserNick(string &token)
{
	size_t pos = token.find_first_of('!');
	return (token.substr(1, pos - 1));
}

std::pair<string, string> ircbot::parseRequest(string &token)
{
	std::pair<string, string>	p;
	size_t 						end;


	size_t pos = token.find_first_not_of(" ");
	if (pos != string::npos && token[pos] == '#')
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

void ircbot::checkOffensiveWords(std::vector<string> &tokens)
{
	string userNick;
	chanIt it;

	// :user5!~x@127.0.0.1 PRIVMSG #c :aaaa
	userNick = getUserNick(tokens[0]);
	it = getChanIt(tokens[2]);
	if (it != _channels.end() && hasBadWords(tokens.back()))
	{
		sendReply("PRIVMSG " + it->_name + " :Please refrain from using inappropriate language, " + userNick + ".\n");
		it->addBadUser(userNick);
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

void ircbot::Channel::updateOperators(std::vector<string> &tokens)
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

void ircbot::Channel::updateUsers(std::vector<string>& tokens)
{
	string user;

	user = getUserNick(tokens[0]);

	if (tokens[1] == "JOIN")
	{
		// if (user == _nick)
		// 	return ;
		User cli(user);
		_loggedUsers.push_back(cli);
	}
	else if (tokens[1] == "KICK" || tokens[1] == "QUIT")
	{
		userIter it =  doesUserExit(user);
		if (it != _loggedUsers.end())
		{
			removeUser(user);
		}
	}
	else if ((tokens[1] == "NICK"))
	{
		userIter it =  doesUserExit(user);
		if (it != _loggedUsers.end())
		{
			updateUserNick(user, tokens.back());
		}
	}
}

ircbot::Channel::Channel(const string &name) : _name(name)
{
}

void ircbot::Channel::logUsers(string &users)
{
	string				user;

	std::stringstream ss(users);
	while (ss >> user)
	{
		// if (user == _nick || "@" + user == _nick)
		// 	continue ;
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

void ircbot::Channel::removeUser(const string &user)
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

void ircbot::Channel::updateUserNick(const string &old_nick, const string &new_nick)
{
	for (userIter it = _loggedUsers.begin(); it < _loggedUsers.end(); it++)
	{
		if (it->_nick == old_nick)
		{
			it->_nick = new_nick;
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

	IRCServRegister();
	// getWeatherInfo();
	while (true)
	{
		// if (!IsBotRunning)
		// 	break;
		handleRead();
		
		while ( (cmd = getCommand()) != "")
		{
			std::cout << "cmd : " << cmd << std::endl;
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
	close(_ircSock);
	// close(_weatherSock);
}
