#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <Channel.hpp>
#include "ircserv.hpp"

#define ERR_NEEDMOREPARAMS string("461")
#define ERR_ALREADYREGISTRED string("462")
#define ERR_PASSWDMISMATCH string("464")
#define ERR_NONICKNAMEGIVEN string("431")
#define ERR_ERRONEUSNICKNAME string("432")
#define ERR_NICKNAMEINUSE string("433")
#define ERR_NICKCOLLISION string("436")
#define ERR_NOTREGISTERED string("451")
#define ERR_UNKNOWNCOMMAND string("421")
#define ERR_NORECIPIENT string("411")
#define ERR_NOTEXTTOSEND string("412")
#define ERR_NOSUCHNICK string("401")
#define ERR_TOOMANYTARGETS string("407")
#define ERR_NOSUCHCHANNEL string("403")
#define ERR_CANNOTSENDTOCHAN string("404")
#define ERR_CHANNELISFULL string("471")
#define ERR_INVITEONLYCHAN string("473")
#define ERR_BADCHANNELKEY string("475")
#define RPL_NAMREPLY string("353")
#define RPL_ENDOFNAMES string("366")
#define ERR_NOSUCHCHANNEL string("403")
#define ERR_UNKNOWNMODE string("472")
#define ERR_CHANOPRIVSNEEDED string("482")
#define ERR_USERNOTINCHANNEL string("441")
#define RPL_CHANNELMODEIS string("324")
#define RPL_CREATIONTIME string("329")
#define ERR_NOSUCHCHANNEL string("403")
#define ERR_CHANOPRIVSNEEDED string("482")
#define ERR_USERNOTINCHANNEL string("441")
#define ERR_NOTONCHANNEL string("442")
#define ERR_USERONCHANNEL string("443")
#define ERR_PASSWDMISMATCH string("464")
#define ERR_INVALIDKEY string("525")
#define ERR_KEYALREADYSET string("467")
#define ERR_INPUTTOOLONG string("417")

#define RPL_MOTDSTART string("375")
#define RPL_MOTD string("372")
#define RPL_ENDOFMOTD string("376")

#define RPL_CHANNELMODEIS string("324")
#define RPL_WELCOME string("001")
#define RPL_YOURHOST string("002")
#define RPL_CREATED string("003")
#define RPL_MYINFO string("004")
#define RPL_NOTOPIC string("331")
#define RPL_TOPIC string("332")

#define RPL_INVITING string("341")



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
