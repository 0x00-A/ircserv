#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <set>
# include <map>
# include <vector>

class IRCClient
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

	IRCClient();

public:

	IRCClient( const std::string& ip, int port, int sockfd );

	~IRCClient();

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

IRCClient::IRCClient()
{
}

IRCClient::IRCClient(const std::string &ip, int port, int sockfd)
	: _ip(ip), _port(port), _clifd(sockfd)
{
	_nick = "";
	_user = "";
	_real = "";
	_hostname = "";
	_recvBuf = "";
	_sendBuf = "";
	_isRegistered = false;
	_isOperator = false;
}

IRCClient::~IRCClient()
{
	// leave channels
	// close fd
}

int IRCClient::getSockfd(void)
{
	return (_clifd);
}

void IRCClient::close() const
{
	::close(_clifd);
}

int IRCClient::getPort(void) const
{
	return (_port);
}

std::string IRCClient::getIPAddr(void) const
{
	return (_ip);
}

std::string& IRCClient::rdBuf(void)
{
	return (_recvBuf);
}

std::string& IRCClient::sdBuf(void)
{
	return (_sendBuf);
}


#endif // CLIENT_HPP
