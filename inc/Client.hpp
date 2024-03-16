#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ircserv.hpp"
// # include "message.hpp"

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

#define RPL_WELCOME 001
#define RPL_CHANNELMODEIS 324

class Client
{
	private:
		string 	_ip;
		int 	_port;

		string 	_nick;
		string 	_username;
		string 	_real;
		string 	_hostname;

		bool 	_isOperator;
		bool 	_isRegistered;

		// track the commands
		bool 	_hasPassed;
		bool 	_hasUsedNick;
		bool 	_hasUsedUser;

		string 	_recvBuf;
		std::queue<string> _sendBuf;

		int _clifd;

		std::set<string> 	_channels; // channels user currently in

		Client();

	public:
		bool operator==(const Client &rhs)
		{

			return (this->_clifd == rhs._clifd);
		}

		Client(const string &ip, int port, int sockfd);

		~Client();

		// Add getters and sitters

		// get socket fd
		int getSockfd(void) const;

		void closeSocket(void) const;

		int getPort(void) const;

		string getIPAddr(void) const;

		// get read buffer
		string &rdBuf(void);

		// get send buffer
		std::queue<string> &sdBuf(void);

		///////////////////////////////////////////

		void setNick(string nick);
		void setUsername(string username);

		void setHasPassed(bool value);
		void setHasUsedNick(bool value);
		void setHasUsedUser(bool value);
		bool isConnected();
		bool getHasPassed();
		bool getHasUsedNick();
		bool getHasUsedUser();
		// bool 			isRegistered(void);
		bool checkNick(string &nick);

		const string &getNick(void) const;
		const string &getUsername(void) const;
};

#endif // CLIENT_HPP
