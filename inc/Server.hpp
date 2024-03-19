#ifndef SERVER_HPP
#define SERVER_HPP

# include "ircserv.hpp"

# include <poll.h>
# include <arpa/inet.h>

# include <netdb.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Socket.hpp"

# define SA struct sockaddr

# define RD_BUF_SIZE 512
# define CHANNEL 1
# define CLIENT 2
# define NOSUCHCHANNEL 3

class Server
{

public:

	typedef std::vector<struct pollfd>::iterator pollfdIter;
	typedef std::vector<Client>::iterator clientIter;
	typedef std::map<string, void (Server::*)(Client &)>::iterator cmdmapIter;
	typedef	std::vector<Channel>::iterator channelIter;
	typedef std::pair<string, string> strPair;

private:

	const string				_port;
	const string				_passwd;
	int		 					_servfd;
	std::vector<struct pollfd>	_pollfds;
	std::vector<Client>			_clients;
	Socket						_socket;

	// just added
	std::vector<string> 					_params;
	std::vector<std::pair<string, int> >	_sendMsgClient;
    std::vector<std::pair<string, string> >	_parsChannels;
	std::vector<string>						_keys;
	string									_messagClient;
	std::map<string, void (Server::*)(Client&)> commandMap;

	//
	std::vector<Channel> 		_channels;
	void broadcastMsg( Client &sender, const string& msg, const Channel& chan );

	int			handleNewConnection( void );
	int			handleRead( int id );
	int			handleWrite( int id );
	string		getCommand( int id );
	void		disconnectClient( int id );
	void		cleanUnusedClients( void );
	void		closeAllOpenSockets( void );
	int			getIndexOfClient( const clientIter& currIter );
	int			getIndexOfClient( const Client& cli );
	clientIter	getClientIterator( const Client& cli );

	/***********************[ MODE ]***********************/
	bool 		parseModes( std::queue< std::pair<string, string> >& modes, Client& cli );

	void		handleOperatorFlag( strPair&, string&, string&, channelIter&, Client& );
	void		handleLimitFlag( strPair&, string&, string&, channelIter& );
	void		handlePasskeyFlag( strPair&, string&, string&, channelIter& );
	void		handleInviteFlag( strPair&, string&, channelIter& );
	void		handleTopicFlag( strPair&, string&, channelIter& );
	void		removeExtraPlusMinus( string& );


	/***********************[ SERVER ]***********************/
	void		parseargs( void ) const;

public:

	Server( const string& port, const string& passwd );

	~Server();

	void		run();

	// functions for debuging
	void	printClients( void );
	void	printpollfds( void );

	// ============================================================ //
	 // parser functions
        // bool parseCommandClient(char *buffer, Client& client);
        void 		handleCommand(string& cmd, int id);
		void 		parseCommand(string& cmd);
		string 		trim_comma(const string &str);
		void		initPrivmsg(Client &client);
		void 		initJoin(Client &client);


        // check nick clients
        bool	checkAlreadyNick(string &nick);
		void	checkSpamClient(Client& client);
		// bool checkNickFormeClient(Client &client);

        // command member functions
        void	pass(Client& client);
        void	user(Client& client);
        void	nick(Client& client);
		void	quit(Client& client);
		void	join(Client& client);
		void	privmsg(Client& client);

		void	mode(Client& client);

        // channel member functions

        void addChannel(string channelName, Client &client);
        void joinChannel(Client& client, std::pair<string, string> channel);
        void leaveChannel(Client& client, string channelName);
		string clientIdentifier(Client &client, string& ch, Channel& channel);
		void	joinedAChannel(Client& client, Channel& channel);
        // void listChannels();
		channelIter	doesChannelExist( const string& chan );
		clientIter	doesUserExit( const string& nick );

        // send messg
		void 	reply(Client &client, string const& reply);

};

#endif //SERVER_HPP
