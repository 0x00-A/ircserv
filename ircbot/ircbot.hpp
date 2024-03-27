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

# define BUF_SIZE 513

using std::string;

class ircbot
{
private:

	// int		_ircSock;
	int		_botSock;
	string	_passwd;
	string	_ircPort;
	std::string	_recvbuf;
	string	_botNick;



public:

	~ircbot();

	ircbot( string, string, string );

	void	connectToServer( void );

	void	run( void );

};

ircbot::ircbot(string passwd, string port, string nick) : _passwd(passwd), _ircPort(port), _recvbuf(""), _botNick(nick)
{
	if ( (_botSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		exit(1);
	}
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
}

void	ircbot::run()
{
	//
	ssize_t 			read_size;
	char				buffer[BUF_SIZE];
	std::vector<string> tokens;
	string				response;

	string msg = "pass " + _passwd + "\nnick " + _botNick + "\nuser x x x x\n";
	write(_botSock, msg.data(), msg.length());
	while (true)
	{
		read_size = read(_botSock, buffer, sizeof(buffer) - 1);
		if (read_size <= 0)
		{
			std::perror("read");
			// close
			break;
		}
		buffer[read_size] = '\0';
		_recvbuf += buffer;
		std::cout << "buf: " << _recvbuf << std::endl;
		
		size_t pos;
		while ( (pos = _recvbuf.find("\n")) != std::string::npos)
		{
			string cmd = _recvbuf.substr(0, pos);
			std::cout << "cmd: " << cmd << std::endl;

			// parse command
				if (cmd.size() > 1 && cmd[cmd.size() - 1] == '\r')
				{
					cmd[cmd.size() - 1] = '\n';
				}
				std::stringstream ss(cmd);
				string	token;
				while (ss >> token)
				{
					tokens.push_back(token);
				}
				if (tokens[1] == "433")
				{
					std::cout << "Nickname is already in use" << std::endl;
					return;
				}
				else if (tokens.size() >= 4 && tokens[1] == "INVITE")
				{
					std::cout << tokens[2] << " have been invited to channel " << tokens[3] << std::endl;
					response = "JOIN " + tokens[3];
					write(_botSock, response.data(), response.length());
				}
				// if (cmd.find())
			// cut cmd from buffer
			_recvbuf = _recvbuf.substr(pos + 1);
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