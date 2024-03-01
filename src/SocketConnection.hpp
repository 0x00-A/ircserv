#ifndef SOCKETCONNECTION_HPP
#define SOCKETCONNECTION_HPP

class SocketConnection
{
private:
	bool	isServ;
	int		sockfd;
public:
	SocketConnection(/* args */);
	~SocketConnection();

	SocketConnection();

	int	getSocketfd();
};

SocketConnection::SocketConnection(/* args */)
{
}

SocketConnection::~SocketConnection()
{
}


#endif //SOCKETCONNECTION_HPP