
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"
#include <ctime>

class Channel
{
	private:

		string						_name;
		std::set<string>			_users;
		std::set<string>			_operators;
		
		string						_modes;	// although there are booleans to check if a certain flag is set i used this also save order of flags in channel when printing


		string						_topic;
		int							_userLimit;		// or string?
		string						_passkey;

		bool						_hasInvite;
		bool						_hasPasskey;
		bool						_hasLimit;
		bool						_hasTopic;
		string 						_creationTime;

		void						setCreationTime( void );


	public:

		Channel(const string& channelName, const string& admin);
		
		~Channel();

		bool				joinUser( const string& user );
		bool				partUser( const string& user );

		bool				isUserInChannel( const string& user ) const;

		bool				isUserOperator( const string& user ) const;	// -o
		
		bool				setChannelOperator( const string& user );
		bool				unsetChannelOperator( const string& user );
		
		bool				setMode(const string& mode);
		bool				hasMode( char mode ) const;
		string				channelModeIs( void ) const;

		void				setTopic(const string& topic);
		void				setPasskey( const string& key );
		void				setUserLimit( string limit );
		// void				setInviteOnly();
	
		void				unsetTopic( void );
		// void				unsetPasskey( const string& key );
		// void				unsetUserLimit( int limit );
		// void				unsetInviteOnly();

		size_t				getSize() const;
		string				getName() const;
		string				getModes( void ) const;
		std::set<string>	getUserList( void ) const;
		std::set<string>	getOperatorList() const;
		string				getPasskey( void ) const;
		size_t				getUserLimit( void ) const;
		string				getTopic( void ) const;
		string				getCreationTime( void ) const;


		bool				empty( void ) const;
		bool				hasPasskey( void ) const;
		bool				hasUserLimit( void ) const;
		bool				hasInvite( void) const;
		bool				hasTopic( void) const;

		void				setHasPasskey( bool stat );
		void				setHasUserLimit( bool stat );
		void				setHasInvite( bool stat );
		void				setHasTopic( bool stat );
		


		void				printUsers( void );
		void				printOperators( void );

};

#endif  // CHANNEL_HPP