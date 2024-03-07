/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:53:31 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 15:08:33 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(void) : _commandsReceived(0), hasPassed(false), hasUsedNick(false), hasUsedUser(false)
{
    this->_clientSocket = -1;
    this->_clientPollfd.fd = -1;
    this->_clientPollfd.events = 0;
    this->_clientPollfd.revents = 0;
    this->_status = 0;
    this->_nick = "*";
    this->_username = "*";
}

Client::Client(Client const &src)
{
    *this = src;
}

Client::~Client(void)
{
}

Client &Client::operator=(Client const &rhs)
{
    if (this != &rhs)
    {
        this->_clientSocket = rhs._clientSocket;
        this->_clientAddress = rhs._clientAddress;
        this->_clientPollfd = rhs._clientPollfd;
        this->_nick = rhs._nick;
        this->_username = rhs._username;
        this->_status = rhs._status;
        this->_buffer = rhs._buffer;
    }
    return *this;
}

void Client::setClientSocket(int const &socket)
{
    this->_clientSocket = socket;
}

void Client::setClientAddress(struct sockaddr_in const &address)
{
    this->_clientAddress = address;
}

void Client::setClientPollfd_fd(int const &fd)
{
    this->_clientPollfd.fd = fd;
}

void Client::setClientPollfd_events(short const &events)
{
    this->_clientPollfd.events = events;
}

void Client::setNick(std::string nick)
{
    this->_nick = nick;
}

void Client::setUsername(std::string username)
{
    this->_username = username;
}

void Client::setStatus(int status)
{
    this->_status = status;
}

void Client::appendBuffer(std::string const &str)
{
    this->_buffer.append(str);
}

void Client::print(void)
{
    std::cout << "Client: " << this->_clientSocket << std::endl;
    std::cout << "Nick: " << this->_nick << std::endl;
    std::cout << "Username: " << this->_username << std::endl;
    std::cout << "Status: " << this->_status << std::endl;
    std::cout << "Buffer: " << this->_buffer << std::endl;
}

int &Client::getClientSocket(void)
{
    return this->_clientSocket;
}

struct sockaddr_in &Client::getClientAddress(void)
{
    return this->_clientAddress;
}

pollfd &Client::getClientPollfd(void)
{
    return this->_clientPollfd;
}

std::string &Client::getNick(void)
{
    return this->_nick;
}

std::string &Client::getUsername(void)
{
    return this->_username;
}

int Client::getStatus(void)
{
    return this->_status;
}

std::string &Client::getBuffer(void)
{
    return this->_buffer;
}
// Path: server.cpp

void Client::incrementCommandsReceived(void)
{
    _commandsReceived++;
}

void Client::setHasPassed(bool value)
{
    this->hasPassed = value;
}

void Client::setHasUsedNick(bool value)
{
    this->hasUsedNick = value;
}

void Client::setHasUsedUser(bool value)
{
    this->hasUsedUser = value;
}

bool Client::getHasPassed()
{
    return this->hasPassed;
}

bool Client::getHasUsedNick()
{
    return this->hasUsedNick;
}

bool Client::getHasUsedUser()
{
    return this->hasUsedUser;
}

bool Client::isRegistered(void)
{
    return (this->hasPassed && this->hasUsedNick && this->hasUsedUser);
}

bool Client::checkConnect()
{
    return (hasUsedNick && hasUsedUser && hasPassed);
}

bool Client::checkNick(std::string &nick)
{
    if (nick.empty())
    {
        return false;
    }
    if (!isalpha(nick[0]))
    {
        return false;
    }
    for (size_t i = 1; i < nick.size(); ++i)
    {
        char current = nick[i];
        if (!isalnum(current) &&
            current != '-' && current != '[' && current != ']' &&
            current != '\\' && current != '`' && current != '^' &&
            current != '{' && current != '}')
        {
            return false;
        }
    }
    return true;
}