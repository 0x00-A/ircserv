#ifndef SERVER_HPP
#define SERVER_HPP

#include "ircserv.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Socket.hpp"
#include <poll.h>

#define RD_BUF_SIZE 512

#define NICKLEN 16
#define USERNAMELEN 9
#define CHANNELLEN 50

#define TOPICLEN 10
#define MODES 20
#define CHANMODES "k,l,i,t"
#define MAXCHANNELS 3

#define CHANNEL 1
#define CLIENT 2
#define NOSUCHCHANNEL 3

extern int	isServerUp;

class Server
{

	public:
		typedef std::vector<struct pollfd>::iterator pollfdIter;
		typedef std::vector<Client>::iterator clientIter;
		typedef std::map<string, void (Server::*)(Client &)>::iterator cmdmapIter;
		typedef std::vector<Channel>::iterator channelIter;
		typedef std::pair<string, string> strPair;

	private:

		const string 								_port;
		const string 								_passwd;
		int 										_servfd;
		std::vector<struct pollfd> 					_pollfds;
		std::vector<Client> 						_clients;
		Socket 										_socket;
		string 										_startTime;
		std::vector<string> 						_params;
		std::vector<std::pair<string, int> > 		_sendMsgClient;
		std::vector<std::pair<string, string> > 	_parsChannels;
		std::vector<string> 						_keys;
		string 										_messagClient;
		string										_servname;

		std::map<string, void (Server::*)(Client &)> commandMap;

		//
		std::vector<Channel> _channels;

		void broadcastMsg(Client &sender, const string &msg, const Channel &chan);

		int 			handleNewConnection(void);
		int 			handleRead(int id);
		int 			handleWrite(int id);
		string 			getCommand(int id);
		void 			disconnectClient(int id);
		void 			cleanDisconnectdClients(void);
		void 			closeAllOpenSockets(void);
		int 			getIndexOfClient(const clientIter &currIter);
		int 			getIndexOfClient(const Client &cli);
		clientIter 		getClientIterator(const Client &cli);
		clientIter 		getClientIterator(const string &nick);
		string 			getMembers(Channel &ch);
		int				getUnknownConnections();
		int				getKnownConnections();
		int				getExistingChannels();

		/***********************[ MODE ]***********************/
		bool 			parseModes( std::queue<std::pair<string, string> > &modes, Client &cli );

		void 			handleOperatorFlag( strPair &, string &, string &, channelIter &, Client & );
		void 			handleLimitFlag( strPair &, string &, string &, channelIter &, Client & );
		void 			handlePasskeyFlag( strPair &, string &, string &, channelIter & );
		void 			handleInviteFlag( strPair &, string &, channelIter & );
		void 			handleTopicFlag( strPair &, string &, channelIter &, Client & );
		void 			removeExtraPlusMinus( string & );
		bool			badFormKey( string & );
		bool			checkValidLimit( string & );

		/***********************[ SERVER ]***********************/
		void 			parseargs(void) const;

	public:

		Server(const string &port, const string &passwd);
		~Server();

		void run();
		// for get time in start
		void 			setStartTime(void);
		string 			getStartTime(void) const;

		void 			welcomeClient(Client &client);

		// ============================================================ //
		// parser functions
		void 			handleCommand(string &cmd, int id);
		void 			parseCommand(string &cmd);
		string 			trim_comma(const string &str, int flg);
		void 			initPrivmsg(Client &client);
		void 			initJoin(Client &client);

		// check nick clients
		bool 			checkAlreadyNick(string &nick);
		void 			checkSpamClient(Client &client);
		void 			changeNick(Client &client);
		// fun reply
		// void    replyNotConnected(Client &client);


		// command member functions
		void 			pass(Client &client);
		void 			user(Client &client);
		void 			nick(Client &client);
		void 			quit(Client &client);
		void 			join(Client &client);
		void 			privmsg(Client &client);
		void 			notice(Client &client);
		void 			mode(Client &client);
		void 			names(Client &client);
		void 			lusers(Client &client);
		void			motd(Client& client);

		void    		kick(Client& client);
		void 			invite(Client& client);
		void 			topic(Client& client);
		

		// channel member functions

		void 			joinChannel(Client &client, std::pair<string, string> channel);
		void 			channelWelcomeMessages(Client &client, Channel &channel);
		void 			joinedAChannel(Client &client, Channel &channel);
		
		channelIter 	doesChannelExist(const string &chan);
		clientIter 		doesUserExit(const string nick);
		void 			removeUserFromChannel(const string user, const string chan);

		void 			exitUserFromChannels(clientIter cli);
		void			showChannelNames(Client& client, Channel& chan);

		void			broadcastToJoinedChannels( Client& client, string &msg );

		// send messg
		void 			reply(Client &client, string const &reply);

};

#endif // SERVER_HPP
