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
# include <string>
# include <signal.h>
# include <strings.h> // for bzero

extern int ircSock;

# define BUF_SIZE 1024

using std::string;

class IRCbot
{
	private:

		struct	Channel;
		struct	User;

		typedef std::vector<User>::iterator userIter;
		typedef std::vector<string>::iterator strVecIter;
		typedef std::vector<Channel>::iterator chanIt;

		struct	User
		{
				string _nick;
				long _timer;

				User(const string& nick);
		};

		struct Channel
		{
			string					_name;
			std::vector<string> 	_operators;
			std::vector<string>		_badUsers;
			std::vector<User> 		_loggedUsers;

			Channel(const string& name);

			void					logUsers( string& users );
			void					updateOperators( std::vector<string>& tokens );
			void					updateUsers(std::vector<string>& tokens, string& botnick);
			userIter				doesUserExit(const string& user);
			bool					isOperator( string& user );
			bool					isMember( string& user );
			string					getBadUsers( void );
			void					addBadUser( string& user );
			void					removeUser(const string& user);
			void					updateUserNick( const string& old_nick, const string& new_nick );
			long					getTime(const string& user);

			// void					debugInfo( void );

		};

	private:

		int									_weatherSock;
		string								_servaddr;
		string								_ircPort;
		string								_passwd;
		string								_nick;
		std::string							_recvbuf;	
		std::vector<Channel>				_channels;
		std::vector<string> 				_wordlist;
		string								_weatherServIP;

		chanIt								getChanIt( string& name );

		void								checkOffensiveWords( std::vector<string>& tokens );
		bool								hasBadWords( string& str );
		void								sendReply( const string& reply );
		bool								isErrorCode( const string& code );
	
		void								IRCServRegister( void );
		string								getWeatherInfo( string& location  );
		void								handleRead( void );
		string								getCommand( void );
		void								parseCommand( string&, std::vector<string>& tokens );
		void								sendWeatherInfo( string& client_nick, string& location );
		string 								parseInfo( std::string marker, string endMarker, std::string& response );
		void								handleCommand( std::vector<string>& tokens );
		void								updateChannels( std::vector<string>& tokens );
		void								joinChannel( string & );
		void								logtimeReply( string& nick, Channel& chan );
		void								blacklistReply( string& nick, Channel& chan );
		std::pair<string, string>			parseRequest( string& token );
		void								privmsg( std::vector<string>& tokens );

		static string						itos( int num );
		static string						getUserNick( string& token );

	public:

		IRCbot( string, string, string, string );
		~IRCbot();

		void								run( void );
		void								connectToIRCServer( void );
		void								connectToWeatherServer( void );

		static long 						getTimeInMinutes();

};

#endif //IRCBOT_HPP
