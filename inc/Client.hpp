#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include "ircserv.hpp"

#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NICKCOLLISION 436
#define ERR_NOTREGISTERED 451
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_NOSUCHNICK 401
#define ERR_TOOMANYTARGETS 407
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_CHANNELISFULL 471
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define ERR_NOSUCHCHANNEL 403
#define ERR_UNKNOWNMODE 472
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_USERNOTINCHANNEL 441
#define RPL_CHANNELMODEIS 324
#define RPL_CREATIONTIME 329
#define ERR_NOSUCHCHANNEL 403
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_PASSWDMISMATCH 464


#define RPL_CHANNELMODEIS 324
#define RPL_WELCOME 001
#define RPL_YOURHOST 002
#define RPL_CREATED 003
#define RPL_MYINFO 004
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332

#define RPL_INVITING 341



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
		void 				inviteToChannel(string& channelName);
		void 				uninviteFromChannel(string &channelName);
		
		bool 				isConnected();
		
		bool 				isInvitedToChannel( string& channelName ) const;
		// struct tm 			*getStartTimeToClinet();
		bool 				getHasPassed();
		bool 				getHasUsedNick();
		bool 				getHasUsedUser();
		
		string 				identifier( void );

		// bool 			isRegistered(void);
		bool 				checkNick(string &nick);

		std::set<string> 	getChannels(void);
		const string		&getNick(void) const;
		const string		&getUsername(void) const;

};

#endif // CLIENT_HPP
