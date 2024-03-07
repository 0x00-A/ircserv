/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:42:19 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 15:00:59 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <poll.h>
#include <vector>

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



class Client {
	private:
		int _commandsReceived;
		int					_clientSocket;
		struct sockaddr_in	_clientAddress;
		pollfd				_clientPollfd;
		std::string			_nick;
		std::string			_username;
		int					_status;
		std::string			_buffer;

		// track the commands
		bool hasPassed;
		bool hasUsedNick;
		bool hasUsedUser;

	public:
		Client(void);
		Client(Client const& src);
		~Client(void);
		Client&	operator=(Client const& rhs);

		void	setClientSocket(int const& socket);
		void	setClientAddress(struct sockaddr_in const& address);
		void	setClientPollfd_fd(int const&);
		void	setClientPollfd_events(short const&);
		void	setNick(std::string nick);
		void	setUsername(std::string username);
		void	setStatus(int status);
		void setHasPassed(bool value);
		void setHasUsedNick(bool value);
		void setHasUsedUser(bool value);


		void	appendBuffer(std::string const& str);
		void	print(void);


		int&				getClientSocket(void);
		struct sockaddr_in&	getClientAddress(void);
		pollfd&				getClientPollfd(void);


		bool checkNick(std::string &nick);
		bool checkConnect();
		bool getHasPassed();
		bool getHasUsedNick();
		bool getHasUsedUser();
		bool isRegistered(void);
		std::string&		getNick(void);
		std::string&		getUsername(void);
		int					getStatus(void);
		std::string&		getBuffer(void);

		void incrementCommandsReceived(void);

};