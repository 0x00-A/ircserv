#include "Client.hpp"

Client::Client()
{
}

Client::Client(const std::string &ip, int port, int sockfd)
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

Client::~Client()
{
	// leave channels
	// close fd
}

int Client::getSockfd(void)
{
	return (_clifd);
}

void Client::close() const
{
	::close(_clifd);
}

int Client::getPort(void) const
{
	return (_port);
}

std::string Client::getIPAddr(void) const
{
	return (_ip);
}

std::string& Client::rdBuf(void)
{
	return (_recvBuf);
}

std::string& Client::sdBuf(void)
{
	return (_sendBuf);
}
