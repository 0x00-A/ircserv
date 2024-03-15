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
    std::vector<std::pair<string, int> >	_parsChannels;
	string									_messagClient;
	std::map<string, void (Server::*)(Client&)> commandMap;

	//
	std::vector<Channel> 		_channels;
	void broadcastMsg(Client &sender, const string& msg, const Channel& chan);

	int			handleNewConnection();
	int			handleRead(int id);
	int			handleWrite(int id);
	string		getCommand(int id);
	void		disconnectClient(int id);
	void		cleanUnusedClients();
	void		closeAllOpenSockets( void );
	int			getIndexOfClient(const clientIter& currIter);
	int			getIndexOfClient(const Client& cli);
	clientIter	getClientIterator(const Client& cli);

	bool 		getModes(std::queue<char>& modes);
	bool    	ValidMode(char& c);

public:

	Server( const string& port, const string& passwd );

	~Server();

	void		start();

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
        bool checkAlreadyNick(string &nick);
		void checkSpamClient(Client& client);
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
        void joinChannel(Client& client, string channelName);

        // send messg
		void 	reply(Client &client, string const& reply);

};

#endif //SERVER_HPP
