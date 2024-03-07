#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <unistd.h>
# include <set>

class Client
{
private:

	std::string _ip;
	int			_port;
	
	std::string _nick;
	std::string _user;
	std::string _real;
	std::string _hostname;

	bool		_isOperator;
	bool		_isRegistered;

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
	int	getSockfd( void );

	void	close( void )	const;

	int		getPort( void ) const;

	std::string		getIPAddr( void ) const;

	// get read buffer
	std::string&	rdBuf( void );

	// get send buffer
	std::string&	sdBuf( void );

};




#endif // CLIENT_HPP
