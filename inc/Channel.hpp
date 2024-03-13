
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

enum USER_TYPE {
	NORMAL_USER,
	OPERATOR
};

class Channel
{
	private:
		string						_name;
		string						_topic;
		std::set<string>			_users;
		std::set<string>			_operators;

	public:
		Channel(const string& channelName);
		
		~Channel();

		// Channel operations
		bool	joinUser( const string& user );

		bool	isUserInChannel( const string& user ) const;

		bool	isUserOperator( const string& user ) const;
		
		bool	setUserAsOperator( const string& user );
		
		bool	partUser( const string& user );


		// void broadcastMessage(Client &sender, string message);


		// Getters
		string				getName() const;
		std::set<string>	getUserList() const;
		std::set<string>	getOperatorList() const;


		void	printUsers( void );
		void	printOperators( void );
};

#endif  // CHANNEL_HPP