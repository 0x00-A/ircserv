#ifndef IRCBOT_HPP
#define IRCBOT_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <cstdlib>
# include <unistd.h>
# include <arpa/inet.h>
# include <poll.h>
# include <vector>
# include <sstream>

# define BUF_SIZE 1024

using std::string;

class ircbot
{
	public:
		class	User
		{
			public:
				string _nick;
				struct tm *_timer;
				User(const string& nick);
				
				struct tm *getCurrentTime();
		};
	private:

		// int		_ircSock;
		int		_botSock;
		string	_passwd;
		string	_ircPort;
		std::string	_recvbuf;
		string	_nick;
		string	_channel;
		std::vector<string> _operators;

		void	registerBot( void );
		void	handleRead( void );
		string	getCommand( void );
		void	parseCommand( string&, std::vector<string>& );
		void	handleCommand( std::vector<string>& );


		std::vector<User> loggedUsers;
		void	logtime(std::vector<string>& tokens);

	public:

		~ircbot();

		ircbot( string, string, string, string );

		void		connectToServer( void );

		void	run( void );

};


ircbot::User::User(const string& nick) : _nick(nick)
{
	_timer = getCurrentTime();
}
ircbot::ircbot(string passwd, string port, string nick, string chan)
	: _passwd(passwd), _ircPort(port), _recvbuf(""), _nick(nick), _channel(chan)
{
	if ( (_botSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		exit(1);
	}
}

struct tm *ircbot::User::getCurrentTime() 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo;
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
		close(_botSock);
		exit(1);
	}
	std::cout << "Connected to server successfully" << std::endl;
}

void	ircbot::registerBot()
{
	std::vector<string> tokens;
	string				cmd;
	// char				buf[BUF_SIZE];

	std::stringstream ss;
    ss << "PASS " << _passwd << "\r\n";
    ss << "NICK " << _nick << "\r\n";
    ss << "USER " << _nick << " 0 * :" << _nick << "\r\n";
    ss << "JOIN " << _channel << "\r\n";

	write(_botSock, ss.str().data(), ss.str().length());
	// read(_botSock, buf, sizeof(buf));
}

void	ircbot::handleRead()
{
	ssize_t 			read_size;
	char				buffer[BUF_SIZE];


	read_size = read(_botSock, buffer, sizeof(buffer) - 1);
	if (read_size <= 0)
	{
		// std::perror("read");
		// // close fds
		// exit(1);
		throw ("read failed!");
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

		// // parse command
		// 	if (cmd.size() > 1 && cmd[cmd.size() - 1] == '\r')
		// 	{
		// 		cmd[cmd.size() - 1] = '\n';
		// 	}
		// 	std::stringstream ss(cmd);
		// 	string	token;
		// 	while (ss >> token)
		// 	{
		// 		tokens.push_back(token);
		// 	}
		// // handle command
		// 	if (tokens[1] == "433")
		// 	{
		// 		std::cout << "Nickname is already in use" << std::endl;
		// 		return;
		// 	}
		// 	else if (tokens.size() >= 4 && tokens[1] == "INVITE")
		// 	{
		// 		std::cout << tokens[2] << " have been invited to channel " << tokens[3] << std::endl;
		// 		response = "JOIN " + tokens[3] + "\r\n";
		// 		write(_botSock, response.data(), response.length());
		// 	}
		// 	// if (cmd.find())
		// cut cmd from buffer
		_recvbuf = _recvbuf.substr(pos + 1);
	}
	return (cmd);
}

void ircbot::parseCommand(string &cmd, std::vector<string> &tokens)
{
    size_t pos;
    std::stringstream ss;
    string token, tmp = "";

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
	string				response;
	string				usersList;
	string				user;

	// if (tokens[1] == "433")
	// {
	// 	throw ("Nickname is already in use");
	// 	// std::cout << "Nickname is already in use" << std::endl;
	// 	// return;
	// }
	// else if (tokens[1] == "432")
	// {
	// 	throw ("Erroneus nickname");
	// 	// std::cout << "Nickname is already in use" << std::endl;
	// 	// return;
	// }
	// if (tokens[1] == "INVITE")
	// {
	// 	std::cout << tokens[2] << " have been invited to channel " << tokens[3] << std::endl;
	// 	response = "JOIN " + tokens[3] + "\r\n";
	// 	write(_botSock, response.data(), response.length());
	// }
	if (tokens[1] == "461" || tokens[1] == "464" || tokens[1] == "451" || tokens[1] == "432" || tokens[1] == "433"
		|| tokens[1] == "403" || tokens[1] == "473" || tokens[1] == "475" || tokens[1] == "471")
	{
		throw (tokens.back());
	}
	else if (tokens[1] == "353")
	{
		//
		usersList = tokens[5]; // 
		// add to logedusers vector User user("nick");
		std::stringstream ss(tokens.back());
		while (ss >> user)
		{
			User cli(user);
			loggedUsers.push_back(cli);
			if (user[0] == '@')
				_operators.push_back(user.substr(1));
		}
	}
	else if (tokens[1] == "PRIVMSG")
	{
		//
		std::cout << ">>>>>>>>> handle privmsg <<<<<<<<<" << std::endl;
	}
	else if (tokens[1] == "JOIN" || tokens[1] == "KICK")
	{
		logtime(tokens);
	}
	// if (cmd.find())
}

void ircbot::logtime(std::vector<string>& tokens)
{
	(void)tokens;
	// if ([1] == "JOIN")
	// {}
	// else if ([1] == "KICK")
	// {}
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
			// cut cmd from buffer
			tokens.clear();
		}
	}
}

ircbot::~ircbot()
{
	// close(_ircSock);
	close(_botSock);
}


#endif //IRCBOT_HPP