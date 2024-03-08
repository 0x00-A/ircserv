#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <unistd.h>
# include <set>

// for pass
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTRED 462
# define ERR_PASSWDMISMATCH 464
/// for nick
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436
//for user

class Client
{
private:

	std::string _ip;
	int			_port;
	
	std::string _nick;
	std::string _username;
	std::string _real;
	std::string _hostname;

	bool		_isOperator;
	bool		_isRegistered;

	// track the commands
	bool 		_hasPassed;
	bool 		_hasUsedNick;
	bool 		_hasUsedUser;

	std::string _recvBuf;
	std::string _sendBuf;

	int			_clifd;


	std::set<std::string> _channels; // channels user currently in

	Client();

public:

	Client( const std::string& ip, int port, int sockfd );

	~Client();

	// Add getters and sitters

	// get socket fd
	int				getSockfd( void );

	void			close( void )	const;

	int				getPort( void ) const;

	std::string		getIPAddr( void ) const;

	// get read buffer
	std::string&	rdBuf( void );

	// get send buffer
	std::string&	sdBuf( void );

	///////////////////////////////////////////

	void			setNick(std::string nick);
	void			setUsername(std::string username);

	void			setHasPassed(bool value);
	void 			setHasUsedNick(bool value);
	void 			setHasUsedUser(bool value);
	bool 			checkConnect();
	bool 			getHasPassed();
	bool 			getHasUsedNick();
	bool 			getHasUsedUser();
	bool 			isRegistered(void);
	bool			checkNick(std::string &nick);

	std::string&		getNick(void);
	std::string&		getUsername(void);

};




#endif // CLIENT_HPP