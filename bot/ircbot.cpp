#include "IRCbot.hpp"

int IsBotRunning = true;

IRCbot::User::User(const string& nick) : _nick(nick)
{
	_timer = getTimeInMinutes();
}

void IRCbot::sendReply( const string &reply)
{
	string response;

	response = reply + "\r\n";
	if (write(_ircSock, response.data(), response.length()) <= 0)
	{
		std::perror("write");
		throw (string("The bot was unable to send a reply to the server"));
	}
}

bool IRCbot::isErrorCode(const string &code)
{
	return ( code == ERR_NEEDMOREPARAMS || code == ERR_PASSWDMISMATCH || code == ERR_NOTREGISTERED
			|| code == ERR_ERRONEUSNICKNAME || code == ERR_NICKNAMEINUSE || code == ERR_NOSUCHCHANNEL
			|| code == ERR_INVITEONLYCHAN || code == ERR_BADCHANNELKEY || code == ERR_CHANNELISFULL);
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

string IRCbot::itos(int num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

IRCbot::IRCbot(string addr, string port, string passwd, string nick)
	: _servaddr(addr), _ircPort(port), _passwd(passwd), _nick(nick), _recvbuf("")
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

long IRCbot::getTimeInMinutes()
{
	std::time_t currentTime;
	std::time(&currentTime);

	return currentTime / 60;
}

void	IRCbot::connectToIRCServer()
{
	struct sockaddr_in servaddr;

	uint32_t ipInt = inet_addr(_servaddr.c_str());
	if (ipInt == INADDR_NONE)
	{
		std::perror("inet_addr");
		close(_weatherSock);
		throw (string("Invalid IP address for weather server"));
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(_ircPort.c_str()));
	servaddr.sin_addr.s_addr = ipInt;

	// TODO ip addr of irc serv 

	if (connect(_ircSock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		std::perror("connect");
		throw (string("The bot was unable to connect to IRC server"));
	}
	std::cout << "Connected to server successfully" << std::endl;
}

void IRCbot::connectToWeatherServer(void)
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

void	IRCbot::IRCServRegister()
{
	std::vector<string> tokens;
	string				cmd, reply;

	std::stringstream ss;
	ss << "PASS " << _passwd << "\r\n";
	ss << "NICK " << _nick << "\r\n";
	ss << "USER " << _nick << " 0 * :" << _nick << "\r\n";
	// ss << "JOIN " << _channel << "\r\n";
	reply = ss.str();
	sendReply(reply);
}

string IRCbot::getWeatherInfo(string& location)
{
    char buffer[1024] = {0};

	(void)location;
    std::string request = 
        "GET /data/2.5/weather?q=" + location + "&APPID=851bb946327ee4bdc5230fe57cd6439f HTTP/1.1\r\n"
        "Host: 37.139.20.5\r\n"
        "Connection: close\r\n\r\n";

    write(_weatherSock, request.c_str(), request.size());
    read(_weatherSock, buffer, sizeof(buffer)-1);

	return (buffer);
}

void	IRCbot::handleRead()
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

void IRCbot::sendWeatherInfo(string& client_nick, string& location)
{
	connectToWeatherServer();

	if (location.empty())
	{
		sendReply("PRIVMSG " + client_nick + " :No city provided");
		return;
	}
	string	response = getWeatherInfo(location);
	if (response.find("404") != string::npos)
	{
		sendReply("PRIVMSG " + client_nick + " :City not found");
		return;
	}
	std::string res, reply;
    reply = "Weather for " + parseInfo("\"name\":\"", "\"", response) + ", " + \
			parseInfo("\"country\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Coordinates: Longitude " + parseInfo("\"lon\":", "\"", response) + \
			" Latitude " + parseInfo("\"lat\":", "}", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Weather Main: " + parseInfo("\"main\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Weather Description: " + parseInfo("\"description\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
	double d = std::strtod(parseInfo("\"temp\":", ",", response).c_str(), NULL);
	d -= 273.15;
	std::stringstream ss;
	ss << d;
    reply = "- Temperature: " + ss.str() + " Celsius";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Humidity: " + parseInfo("\"humidity\":", "}", response) + "%";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Wind Speed: " + parseInfo("\"speed\":", ",", response) + " m/s";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
	close(_weatherSock);
}

void	IRCbot::privmsg(std::vector<string>& tokens)
{
	string		userNick;
	chanIt		it;
	std::pair<string, string> pair;
	// :user5!~x@127.0.0.1 PRIVMSG bot :hi
	userNick = getUserNick(tokens[0]);
	pair = parseRequest(tokens.back());

	std::cout << "pair : |" << tokens.back() << "| first : |" << pair.first << "| second : |" << pair.second << "|" << std::endl;

	it = getChanIt(pair.first);

	if (tokens[2] == _nick && pair.first == "weather")		// not just a privmsg to bot
	{
		sendWeatherInfo(userNick, pair.second);
	}
	else if (tokens[2] != _nick)		// not just a privmsg to bot
	{
		checkOffensiveWords(tokens);
	}
	else if (tokens[2] == _nick && it == _channels.end())
	{
		sendReply("PRIVMSG " + userNick + " :Usage 1: #channel <logtime/blacklist>");
		sendReply("PRIVMSG " + userNick + " :Usage 2: weather");
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

void	IRCbot::handleCommand(std::vector<string>& tokens)
{
	string				response, usersList, command;
	chanIt				it;

	command = tokens[1];
	if (isErrorCode(command))
	{
		throw (tokens.back());
	}
	else if (tokens[0] == "PING")
	{
		sendReply("PONG " + tokens.back());
	}
	else if (command == "INVITE")
	{
		joinChannel(tokens.back());
	}
	else if (command == RPL_NAMREPLY)
	{
		if ( (it = getChanIt(tokens[4])) != _channels.end())
		{
			it->logUsers(tokens.back());
		}
	}
	else if (command == "PRIVMSG")
	{
		privmsg(tokens);
	}
	else if (command == "QUIT" || command == "NICK")
	{
		updateChannels(tokens);
	}
	else if (command == "JOIN" || command == "KICK")
	{
		it = getChanIt(tokens[2]);
		if (command == "KICK" && tokens[3] == _nick)	// bot kickek from a channel
		{
			_channels.erase(it);
		}
		else
		{
			it->updateUsers(tokens, _nick);
			it->debugInfo();
		}
	}
	else if (command == "MODE" && (tokens[3] == "+o" || tokens[3] == "-o"))
	{
		if ( (it = getChanIt(tokens[2])) != _channels.end())
		{
			it->updateOperators(tokens);
			it->debugInfo();
		}
	}
}

void IRCbot::updateChannels(std::vector<string> &tokens)
{
	if (tokens[1] == "QUIT")
	{
		for (chanIt it = _channels.begin(); it < _channels.end(); it++)
		{
			string nick = getUserNick(tokens[0]);
			std::cout << "nick to remove: " << nick << std::endl;
			it->removeUser(nick);
			it->debugInfo(); // delete
		}
	}
	if (tokens[1] == "NICK")
	{
		for (chanIt it = _channels.begin(); it < _channels.end(); it++)
		{
			string nick = getUserNick(tokens[0]);
			std::cout << "nick to change: " << nick << std::endl;
			it->updateUserNick(nick, tokens[2]);
			it->debugInfo(); // delete
		}
	}
}

void	IRCbot::joinChannel( string& chan )
{
	// :user5!~x@127.0.0.1 INVITE bot :#c
	std::cout << "bot invited to channel " << chan << std::endl;
	sendReply("JOIN " + chan);
	_channels.push_back(Channel(chan));
}

IRCbot::chanIt IRCbot::getChanIt(string &name)
{
	for (chanIt it = _channels.begin(); it < _channels.end(); it++)
	{
		if (it->_name == name)
			return (it);
	}
	return (_channels.end());
}

void	IRCbot::run()
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

IRCbot::~IRCbot()
{
	close(_ircSock);
}
