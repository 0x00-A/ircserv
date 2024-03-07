#include "IRCClient.hpp"

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
