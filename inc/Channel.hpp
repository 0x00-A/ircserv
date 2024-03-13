
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Channel
{
	private:

		string						_name;
		std::set<string>			_users;
		std::set<string>			_operators;
		
		string						_modes;


		string						_topic;
		int							_userLimit;		// or string?
		string						_passkey;

		bool						_hasInvite;
		bool						_hasPasskey;
		bool						_hasLimit;
		bool						_hasTopic;


	public:

		Channel(const string& channelName);
		
		~Channel();

		bool				joinUser( const string& user );
		bool				partUser( const string& user );

		bool				isUserInChannel( const string& user ) const;

		bool				isUserOperator( const string& user ) const;	// -o
		
		bool				setUserAsOperator( const string& user );
		
		bool				setMode(const string& mode);
		bool				hasMode( char mode ) const;

		void				setTopic(const string& topic);
		void				setPasskey( const string& key );
		bool				setUserLimit( string limit );
		void				setInviteOnly();
	
		void				unsetTopic( void );
		// void				unsetPasskey( const string& key );
		// void				unsetUserLimit( int limit );
		// void				unsetInviteOnly();


		string				getName() const;
		string				getModes( void ) const;
		std::set<string>	getUserList( void ) const;
		std::set<string>	getOperatorList() const;
		string				getPasskey( void ) const;
		int					getUserLimit( void ) const;
		string				getTopic( void ) const;

		bool				empty( void ) const;
		bool				hasPasskey( void ) const;
		bool				hasUserLimit( void ) const;
		bool				hasInvite( void) const;
		bool				hasTopic( void) const;

		void				setHasPasskey( void );
		void				setHasUserLimit( void );
		void				setHasInvite( void);
		void				setHasTopic( void);
		


		void				printUsers( void );
		void				printOperators( void );

};

#endif  // CHANNEL_HPP