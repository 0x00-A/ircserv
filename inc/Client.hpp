#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ircserv.hpp"

# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTRED 462
# define ERR_PASSWDMISMATCH 464
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436
# define ERR_NOTREGISTERED 451

class Client
{
private:

	string 				_ip;
	int					_port;
	
	string 				_nick;
	string 				_username;
	string 				_real;
	string 				_hostname;

	bool				_isOperator;
	bool				_isRegistered;

	// track the commands
	bool 				_hasPassed;
	bool 				_hasUsedNick;
	bool 				_hasUsedUser;

	string 				_recvBuf;
	std::queue<string>	_sendBuf;

	int					_clifd;


	std::set<string>	_channels; // channels user currently in

	Client();

public:

	bool operator==(const Client& rhs) {

		return (this->_clifd == rhs._clifd);
	}

	Client( const std::string& ip, int port, int sockfd );

	~Client();

	// Add getters and sitters

	// get socket fd
	int				getSockfd( void ) const;

	void			closeSocket( void )	const;

	int				getPort( void ) const;

	string			getIPAddr( void ) const;

	// get read buffer
	string&	rdBuf( void );

	// get send buffer
	std::queue<string>&	sdBuf( void );

	///////////////////////////////////////////

	void			setNick(string nick);
	void			setUsername(string username);

	void			setHasPassed(bool value);
	void 			setHasUsedNick(bool value);
	void 			setHasUsedUser(bool value);
	bool 			checkConnect();
	bool 			getHasPassed();
	bool 			getHasUsedNick();
	bool 			getHasUsedUser();
	bool 			isRegistered(void);
	bool			checkNick(string &nick);

	string&		getNick(void);
	string&		getUsername(void);

};




#endif // CLIENT_HPP
