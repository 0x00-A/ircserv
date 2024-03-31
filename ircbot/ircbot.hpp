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
# include "numReplies.hpp"
# include <algorithm>
# include <ctime>
# include <sys/types.h>
# include <strings.h>

# define BUF_SIZE 1024

using std::string;

class ircbot
{
	private:

		class	User
		{
			public:
				string _nick;
				long _timer;
				User(const string& nick);
		};

		typedef std::vector<User>::iterator userIter;
		typedef std::vector<string>::iterator strVecIter;

	private:

		int						_botSock;
		string					_passwd;
		string					_ircPort;
		std::string				_recvbuf;
		string					_nick;
		string					_channel;
		std::vector<string> 	_operators;
		std::vector<string> 	_wordlist;
		std::vector<User> 		_loggedUsers;
		std::vector<string>		_badUsers;

		void					registerBot( void );
		void					handleRead( void );
		string					getCommand( void );
		void					parseCommand( string&, std::vector<string>& );
		void					handleCommand( std::vector<string>& );
		string					getUserNick( string& token );
		void					checkOffensiveWords( std::vector<string>& tokens );
		bool					hasBadWords( string& str );
		void					updateOperators( std::vector<string>& tokens );
		void					updateUsers(std::vector<string>& tokens);
		void					logUsers( string& users );

		void					blacklistReply( string& nick );
		void					logtimeReply( string& nick );

		std::string				itos(int num);
		long					getTime(const string& user);
		void					removeUser(const string& user);
		void					updateUserNick( const string& old_nick, const string& new_nick );
		userIter				doesUserExit(const string& user);
		bool					isOperator( string& user );
		bool					isMember( string& user );
		string					getBadUsers( void );
		void					addBadUser( string& user );
		void					sendReply(const string& reply);
		bool					isErrorCode( const string& code );
	
	public:

		ircbot( string, string, string, string );
		~ircbot();

		void					run( void );
		void					connectToServer( void );

		static long 			getTimeInMinutes();

		void	debugInfo( void );
};

#endif //IRCBOT_HPP
