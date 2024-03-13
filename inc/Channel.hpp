
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Channel
{
	private:

		string						_name;
		string						_topic;
		std::set<string>			_users;
		std::set<string>			_operators;
		
		string						_modes;
		bool						_hasPasskey;
		bool						_hasLimit;
		bool						_hasTopic;
		int							_userLimit;		// or string?
		string						_passkey;

	public:

		Channel(const string& channelName);
		
		~Channel();

		// add a user to channel
		bool				joinUser( const string& user );

		// check if a user is member in channel
		bool				isUserInChannel( const string& user ) const;

		// check if a user is an operator
		bool				isUserOperator( const string& user ) const;
		
		// give user operator privilages
		bool				setUserAsOperator( const string& user );
		
		// delets a user from the channel
		bool				partUser( const string& user );


		// moved this to Server
		// void broadcastMessage(Client &sender, string message);

		// still not sure how to implement this
		bool				setMode(const string& mode, const string& param);

		void				setPasskey( const string& key );
		void				setUserLimit( int limit );

		// returns channel name
		string				getName() const;

		// return a list of channel users
		std::set<string>	getUserList() const;

		// return a list channel operators
		std::set<string>	getOperatorList() const;

		
		string				getPasskey( void );

		int					getUserLimit( void );
		// returns if the channel is empty or not
		bool				empty( void ) const;

		bool				hasPasskey( void ) const;
		bool				hasUserLimit( void ) const;
		bool				hasMode( const string& mode ) const;


		void				printUsers( void );
		void				printOperators( void );

};

#endif  // CHANNEL_HPP