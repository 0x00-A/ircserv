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
# include <fstream>
# include <iomanip>

# define BUF_SIZE 1024

using std::string;

class ircbot
{
	private:

		class	User
		{
			public:
				string _nick;
				int _timer;
				User(const string& nick);
		};

		typedef std::vector<User>::iterator userIter;
		typedef std::vector<string>::iterator opIter;

	private:

		int						_botSock;
		string					_passwd;
		string					_ircPort;
		std::string				_recvbuf;
		string					_nick;
		string					_channel;
		std::vector<string> 	_operators;
		std::vector<string> 	_wordlist;
		std::vector<User> 		loggedUsers;

		void					registerBot( void );
		void					handleRead( void );
		string					getCommand( void );
		void					parseCommand( string&, std::vector<string>& );
		void					handleCommand( std::vector<string>& );
		string					getUserNick( string& token );
		void					checkOffensiveWords( std::vector<string>& tokens );
		bool					hasBadWords( string& str );
		void					updateOperators( std::vector<string>& tokens );
		void					logtime(std::vector<string>& tokens);

		std::string				itos(int num);
		int						getTime(const string& user);
		void					removeUser(const string& user);
		userIter				doesUserExit(const string& user);
	
	public:

		ircbot( string, string, string, string );
		~ircbot();

		void					run( void );
		void					connectToServer( void );

		static struct tm 		*getCurrentTime();

};

#endif //IRCBOT_HPP
