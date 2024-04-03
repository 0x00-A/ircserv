#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include "ircserv.hpp"

class Client
{
	private:

		string 					_ip;
		int 					_port;
		string 					_nick;
		string 					_username;
		string 					_real;
		string 					_hostname;

		// for time 
		// struct tm*				_startTime;

		// bool 	_isOperator;
		// bool 	_isRegistered;

		// track the commands
		bool 					_hasPassed;
		bool 					_hasUsedNick;
		bool 					_hasUsedUser;
		string 					_recvBuf;
		std::queue<string> 		_sendBuf;
		int 					_clifd;
		std::vector<string> 	_invitedChannels; // here add chan name and invite status ture or false 
		std::set<string> 		_channels; // channels user currently in

		Client();

	public:

		Client(const string &ip, int port, int sockfd);
		~Client();

		// Add getters and sitters

		// get socket fd
		int 				getSockfd(void) const;

		void 				closeSocket(void) const;

		int 				getPort(void) const;

		string 				getIPAddr(void) const;

		// get read buffer
		string 				&rdBuf(void);

		// get send buffer
		std::queue<string>	&sdBuf(void);

		///////////////////////////////////////////


		void 				addChannels(string &channel);
		void 				setNick(string nick);
		void 				setUsername(string username);

		// void 				setStartTimeToClinet();
		void 				setHasPassed(bool value);
		void 				setHasUsedNick(bool value);
		void 				setHasUsedUser(bool value);
		void 				inviteToChannel(const string& channelName);
		void 				uninviteFromChannel(const string& channelName);
		bool 				isInvitedToChannel( const string& channelName ) const;
		
		bool 				isConnected();
		
		// struct tm 			*getStartTimeToClinet();
		std::vector<string> 	getInvitedChannels( void ) const;
		bool 					getHasPassed();
		bool 					getHasUsedNick();
		bool 					getHasUsedUser();
			
		string 					identifier( void );

		// bool 			isRegistered(void);
		bool 				checkNick(string &nick);

		std::set<string> 	getChannels(void);
		const string		&getNick(void) const;
		const string		&getUsername(void) const;

};

#endif // CLIENT_HPP
