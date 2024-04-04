#include "IRCbot.hpp"

int ircSock = -1;

IRCbot::IRCbot(string addr, string port, string passwd, string nick)
	: _servaddr(addr), _ircPort(port), _passwd(passwd), _nick(nick), _recvbuf("")
{
	_weatherServIP = "37.139.20.5";
	
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

void	IRCbot::connectToIRCServer()
{
	struct sockaddr_in servaddr;

	if ( (ircSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		exit(1);
	}

	uint32_t ipInt = inet_addr(_servaddr.c_str());
	if (ipInt == INADDR_NONE)
	{
		std::perror("inet_addr");
		throw (string("Invalid IP address for IRC server"));
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(std::atoi(_ircPort.c_str()));
	servaddr.sin_addr.s_addr = ipInt;

	if (connect(ircSock, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		std::perror("connect");
		throw (string("The bot was unable to connect to IRC server"));
	}
	std::cout << "Connected to IRC server successfully" << std::endl;
}

void	IRCbot::IRCServRegister()
{
	std::vector<string> tokens;
	string				cmd, reply;

	std::stringstream ss;
	ss << "PASS " << _passwd << "\r\n";
	ss << "NICK " << _nick << "\r\n";
	ss << "USER " << _nick << " 0 * :" << _nick << "\r\n";
	reply = ss.str();
	sendReply(reply);
}

void	IRCbot::handleRead()
{
	ssize_t 			read_size;
	char				buffer[BUF_SIZE];

	read_size = read(ircSock, buffer, sizeof(buffer) - 1);
	if (read_size <= 0)
	{
		std::perror("read");
		throw (string("server stoped running!"));
	}
	buffer[read_size] = '\0';
	_recvbuf += buffer;
}

void	IRCbot::privmsg(std::vector<string>& tokens)
{
	string						userNick;
	chanIt						it;
	std::pair<string, string>	pair;

	userNick = getUserNick(tokens[0]);
	pair = parseRequest(tokens.back());
	it = getChanIt(pair.first);
	if (tokens[2] != _nick)	// not a privmsg to bot
	{
		checkOffensiveWords(tokens);
	}
	else if (tokens[2] == _nick && pair.first == "weather")
	{
		sendWeatherInfo(userNick, pair.second);
	}
	else if (tokens[2] == _nick && pair.second == "logtime")
	{
		if (it != _channels.end())
			logtimeReply(userNick, *it);
	}
	else if (tokens[2] == _nick && pair.second == "blacklist")
	{
		if (it != _channels.end())
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
		if (command == "KICK" && tokens[3] == _nick) // bot kickek from a channel
		{
			_channels.erase(it);
		}
		else
		{
			it->updateUsers(tokens, _nick);
		}
	}
	else if (command == "MODE" && (tokens[3] == "+o" || tokens[3] == "-o"))
	{
		if ( (it = getChanIt(tokens[2])) != _channels.end())
		{
			it->updateOperators(tokens);
		}
	}
}

void	IRCbot::run()
{
	std::vector<string> tokens;
	string				response, cmd;

	IRCServRegister();
	while (true)
	{
		handleRead();
		
		while ( (cmd = getCommand()) != "")
		{
			std::cout << cmd << std::endl;
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
	close(ircSock);
}
