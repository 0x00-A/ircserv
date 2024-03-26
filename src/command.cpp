#include "Server.hpp"
#include "Channel.hpp"


void Server::pass(Client &client)
{
    if (client.isConnected())
    {
        throw (":ft_irc.1337.ma " + itos(ERR_ALREADYREGISTRED) + " " + client.getNick()  + " :You may not reregister");
    }
    if (this->_params.size() < 2)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick()  + " " + this->_params[0] + " \
                :Not enough parameters");
    }
    if (this->_params[1] == this->_passwd)
    {
        client.setHasPassed(true);
    }
    else
    {
        client.setHasPassed(false);
        throw (":ft_irc.1337.ma " + itos(RPL_WELCOME) + " " + client.getNick()  + " " + this->_params[1] + \
                " :Password incorrect");
    }
}

void Server::nick(Client &client)
{
    bool    welcome = false;

    if (client.getHasPassed() == false)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick()  + " :You have not registered");
    }
    if (this->_params.size() < 2)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NONICKNAMEGIVEN) + " " + client.getNick()  + " :No nickname given");
    }
    if (this->_params[1].size() >= 16)
    {
        this->_params[1].erase(15);
    }
    if (client.checkNick(this->_params[1]) == false)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_ERRONEUSNICKNAME) + " " + client.getNick()  + " :Erroneus nickname");
    }
    if (checkAlreadyNick(this->_params[1]) == false)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NICKNAMEINUSE) + " " + client.getNick()  + " :Nickname is already in use");
    }
    if (client.getHasUsedNick() == true && !client.isConnected())
    {
        reply(client, client.identifier() + " NICK :" +  this->_params[1]);
    }
    if (client.isConnected())
    {
        changeNick(client);
        welcome = true;
    }
    client.setNick(this->_params[1]);
    client.setHasUsedNick(true);
    if (client.isConnected() && !welcome) welcomeClient(client);
    if (_clients.size() > 1 && client.getHasUsedUser())
    {
        checkSpamClient(client);
    }
}


void Server::user(Client &client)
{
    if (client.isConnected())
    {
        throw (":ft_irc.1337.ma " + itos(ERR_ALREADYREGISTRED) + " " + client.getNick()  + " :You may not reregister");
    }
    if (client.getHasPassed() == false)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick()  + " :You have not registered");
    }
    if (this->_params.size() < 5)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick()  + " " + this->_params[0] + \
                " :Not enough parameters");
    }
    if (this->_params[1].size() > 9) this->_params[1].erase(9);
    client.setHasUsedUser(true);
    client.setUsername(this->_params[1]);
    if (_clients.size() > 1 && client.getHasUsedNick())
    {
        checkSpamClient(client);
    }
    if (client.isConnected())  welcomeClient(client);
}

void Server::quit(Client &client)
{
    string              response;
    std::set<string>    channelsJ;

    response = client.identifier() + " QUIT :Client Quit";
    channelsJ = client.getChannels();

    std::set<string>::iterator it = channelsJ.begin();
    for ( ; it != channelsJ.end(); it++)
    {
        channelIter itCha = doesChannelExist(*it);
        if (itCha != _channels.end())
        {
            this->broadcastMsg(client, response, *itCha);
        }
    }
    response += "\r\n";
    write(client.getSockfd(), response.c_str(), response.length());
    response = "ERROR :Closing Link: " + client.getIPAddr()  + " (Client Quit)\r\n";
    write(client.getSockfd(), response.c_str(), response.length());
    _pollfds[getIndexOfClient(client) + 1].fd = -1;
}

void Server::join(Client &client)
{
    string  response;

    if (!client.isConnected())
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick()  + \
                " :You have not registered");
    }
    initJoin(client);
    for (size_t i = 0; i < _parsChannels.size(); i++)
    {
        if (_parsChannels[i].first[0] != '#')
        {
            response = ":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + " " + client.getNick() + " " + _parsChannels[i].first + \
                    " :No such channel";
            reply(client, response);
        }
        else
        {
            joinChannel(client, _parsChannels[i]);
        }
    }

}


void Server::privmsg(Client &client)
{
    std::set<string>    seenNicks;
    string              response;
    channelIter         chanIt;
    clientIter          cliIt;

    initPrivmsg(client);
    for (size_t i = 0; i < _sendMsgClient.size(); i++)
    {
        if (seenNicks.count(_sendMsgClient[i].first) > 0) 
        {
            response = ":ft_irc.1337.ma " + itos(ERR_TOOMANYTARGETS) + " " +  client.getNick() + \
                        " :Duplicate recipients";
            reply(client, response);
            continue ;
        }
        seenNicks.insert(_sendMsgClient[i].first);
        if (_sendMsgClient[i].first[0] == '#')
        {
            if ( (chanIt = doesChannelExist(_sendMsgClient[i].first)) != _channels.end())
            {
                response = ":"  + client.getNick() + "!~" + client.getUsername()  + \
                    "@" + client.getIPAddr() + " PRIVMSG " + chanIt->getName() + " :" + _messagClient;
                broadcastMsg(client, response, *chanIt);
            }
            else
            {
                response = (":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + \
                " " +  client.getNick() + " " + _sendMsgClient[i].first + " :No such channel");
                reply(client, response);
            }
        }
        else
        {
            if ( (cliIt = doesUserExit(_sendMsgClient[i].first)) != _clients.end())
            {
                response = client.identifier() + " PRIVMSG " + cliIt->getNick() + " :" +  _messagClient;
                reply(*cliIt, response);
            }
            else
            {
                response = ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + \
                " " +  client.getNick() + " " + _sendMsgClient[i].first + " :No such nick/channel";
                reply(client, response);
            }
        }
    }   
}


/********************************************************************************/
/* The MODE command is provided so that channel operators may change the        */
/* characteristics of `their' channel.                                          */
/********************************************************************************/

// MODES

//	o - give/take channel operator privileges;
//	i - invite-only channel flag;
//	t - topic settable by channel operator only flag;
//	l - set the user limit to channel;
//	k - set a channel key (password).

// Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]

// NOTES
//	When using the 'o' options, a restriction on a total of three per mode command has been imposed

/* Examples
	MODE #Finnish +i               ; Makes #Finnish channel 'invite-only'.
	MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on channel #Finnish.
	MODE #42 +k oulu                ; Set the channel key to "oulu".
	MODE #eu-opers +l 10            ; Set the limit for the number of users on channel to 10.
*/
/********************************************

 * if the channel already has the mode nothing happens

MODE
:zinc.libera.chat 461 rimi MODE :Not enough parameters
MODE eer
:zinc.libera.chat 403 rimi eer :No such channel
MODE #gggg +l/+o
:zinc.libera.chat 461 rimi MODE :Not enough parameters

MODE #gggg df
:zinc.libera.chat 472 rimi d :is an unknown mode char to me

MODE #gggg l key
MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnst

MODE #gggg +o fff
:zinc.libera.chat 401 rimi fff :No such nick/channel

MODE #gggg +l 123
:rimi!~x@197.230.30.146 MODE #gggg +l 123

MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnstl 123
:zinc.libera.chat 329 rimi #gggg 1710302483

MODE #gggg -t
:rimi!~x@197.230.30.146 MODE #gggg -t

MODE #gggg -l
:rimi!~x@197.230.30.146 MODE #gggg -l

// unessecery params are ignored
MODE #gggg +l 123 ddfdfffd
:rimi!~x@197.230.30.146 MODE #gggg +l 123
MODE #gggg
:zinc.libera.chat 324 rimi #gggg +Cnstl 123
:zinc.libera.chat 329 rimi #gggg 1710302483


mode #ch1 --t+++i
:kkk!~x@197.230.30.146 MODE #ch1 -t+i

********************************************/

bool    Server::parseModes(std::queue< std::pair<string, string> >& modes, Client& client)
{
    string sign = "+";
    for (size_t i = 0, k = 3; i < _params[2].size(); i++)
    {
        if (_params[2][i] == '-' || _params[2][i] == '+')
        {
            sign = _params[2].substr(i, 1);
            continue;
        }
        char c = _params[2][i];
        if (c == 'o' || c == 'l' || c == 'k' || c == 't' || c == 'i')
        {
            if ( (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+")) && !(k < _params.size()))
            {
                throw ( ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + \
                client.getNick() + " " + _params[0]  + " :Not enough parametersss" );
            }
            if (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+"))
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), _params[k++]));
            else
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), ""));
        }
        else
        {
            throw ( ":ft_irc.1337.ma " + itos(ERR_UNKNOWNMODE) + " " + client.getNick() + \
            " " + _params[2].substr(i, 1) + " :is an unknown mode char to me" );
        }
    }
    return (true);
}

void Server::handleOperatorFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan, Client& cli)
{
    if (doesUserExit(m.second) == _clients.end())
    {
        reply(cli, ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + " " + \
            cli.getNick() + " " + m.second + " :No such nick");
    }
    else if (!chan->isUserInChannel(m.second))
    {
        reply(cli, ":ft_irc.1337.ma " + itos(ERR_USERNOTINCHANNEL) + " " + \
            cli.getNick() + " " + _params[1] + " :They aren't on that channel");
    }
    else
    {
        (m.first == "+o")? chan->setChannelOperator(m.second) : chan->unsetChannelOperator(m.second);
        // :u1!~x@197.230.30.146 MODE #ch1 -o u4
        broadcastMsg(cli, cli.identifier() + " MODE " + _params[1] + " " + m.first + " " + m.second, *chan);
        modesave += m.first;
        paramsave += " " + m.second;
    }
}

void Server::handleLimitFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan)
{
	if (m.first == "+l")
	{
		if (std::atoi(m.second.c_str()) > 0)
		{
			chan->setMode(m.first);
			chan->setHasUserLimit(true);
			chan->setUserLimit(m.second);
			modesave += m.first;
			paramsave += " " + m.second;
		}
	}
	else if (m.first == "-l")
	{
		if (chan->setMode(m.first))
		{
			chan->setHasUserLimit(false);
			modesave += m.first;
		}
	}
}

void Server::handlePasskeyFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan)
{
	if (m.first == "+k")
	{
		chan->setMode(m.first);
		chan->setPasskey(m.second);
		modesave += m.first;
		paramsave += " " + m.second;
	}
	else if (m.first == "-k")
	{
		if (chan->hasPasskey())
		{
			chan->setMode(m.first);
			chan->setHasPasskey(false);
			modesave += m.first;
			paramsave += " *";
		}
	}
}

void Server::handleInviteFlag(strPair &m, string &modesave, channelIter &chan)
{
	if (chan->setMode(m.first))
	{
		(m.first == "+i")? chan->setHasInvite(true) : chan->setHasInvite(false);
		modesave += m.first;
	}
}

void Server::handleTopicFlag(strPair &m, string &modesave, channelIter &chan)
{
    if (chan->setMode(m.first))
    {
        (m.first == "+t") ? chan->setHasTopic(true) : chan->setHasTopic(false);
        modesave += m.first;
    }
}

void Server::removeExtraPlusMinus(string &s)
{
	bool sawPlus = false;
	bool sawMinus = false;

	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '+' && !sawPlus)
		{
			sawPlus = true;
			sawMinus = false;
		}
		else if (s[i] == '+' && sawPlus)
		{
			s.erase(i--, 1);
		}
		if (s[i] == '-' && !sawMinus)
		{
			sawMinus = true;
			sawPlus = false;
		}
		else if (s[i] == '-' && sawMinus)
		{
			s.erase(i--, 1);
		}
	}
}

void    Server::mode(Client& client)
{
    std::queue<std::pair<string, string> >      modes;
    channelIter                                 chan;
    string                                      paramsave;
    string                                      modesave;

    if (!client.isConnected())
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + \
        client.getNick()  + " :You have not registered");
    }
    if (_params.size() < 2)
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + \
        client.getNick() + " " + _params[0]  + " :Not enough parameters");
    }
    if ( (chan = doesChannelExist(_params[1])) == _channels.end())
    {
        throw (":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + " " + \
        client.getNick() + " " + _params[1]  + " :No such channel");
    }
    if (_params.size() == 2)
    {
        reply(client, (":ft_irc.1337.ma " + itos(RPL_CHANNELMODEIS) + " " + \
        client.getNick() + " " + _params[1] + " " + chan->channelModeIs()));
        throw (":ft_irc.1337.ma " + itos(RPL_CREATIONTIME) + " " + \
        client.getNick() + " " + _params[1] + " " + chan->getCreationTime());
    }
    parseModes(modes, client);
    if (!chan->isUserOperator(client.getNick()))
    {
        throw (":ft_irc.1337.ma " + itos(ERR_CHANOPRIVSNEEDED) + " " + \
        client.getNick() + " " + _params[1] + " " + " :You're not channel operator");
	}
	while (!modes.empty())
	{
		std::pair<string, string> m = modes.front();
		switch (m.first[1])
		{
			case ('o'):
			handleOperatorFlag(m, modesave, paramsave, chan, client);
			break;
			case ('l'):
			handleLimitFlag(m, modesave, paramsave, chan);
			break;
			case ('k'):
			handlePasskeyFlag(m, modesave, paramsave, chan);
			break;
			case ('i'):
			handleInviteFlag(m, modesave, chan);
			break;
			case ('t'):
			handleTopicFlag(m, modesave, chan);
			break;
		}
		modes.pop();
	}
	removeExtraPlusMinus(modesave);
	if (!modesave.empty())
		throw (client.identifier() + " " + _params[0] + " " + _params[1] + " " + modesave + paramsave);
}

//////////////////////////////////////////////////////////////////////////

void Server::kick(Client& client)
{
    if (!client.isConnected()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick() + " :You have not registered\r\n");
        return;
    }
    if (_params.size() < 3) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::string channelName = _params[1];
    std::string key = _params[2];
	std::string reason = (_params.size() > 3) ? _params[3] : "No reason provided";

	channelIter chanit = doesChannelExist(channelName);
    if (chanit == _channels.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + " " + client.getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTONCHANNEL) + " " + client.getNick() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }
    if (!chanit->isUserOperator(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_CHANOPRIVSNEEDED) + " " + client.getNick() + " :You're not channel operator\r\n");
        return;
    }

	if (doesUserExit(key) == _clients.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + " " + client.getNick() + " " + channelName + " :No such nick/channel\r\n");
        return;
	}
    if (!chanit->isUserInChannel(key)) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_USERNOTINCHANNEL) + " " + client.getNick() + " " + channelName + " :they are not on that channel\r\n");
        return;
    }
	removeUserFromChannel(key, channelName);
    std::string kickMessage = client.identifier() + " KICK " + channelName + " " + key + " :" + reason;
	broadcastMsg(client, kickMessage, *chanit);

    std::cout << "User " << client.getNick() << " kicked " << key << " from " << channelName << "\n";
}


void Server::invite(Client& client)
{
    if (!client.isConnected()) {
        reply(client, "451 :You have not registered\r\n");
        return;
    }
    if (_params.size() < 3) {
        reply(client, "461 " + client.getNick() + " INVITE :Not enough parameters\r\n");
        return;
    }
    std::string userToInvite = _params[1];
    std::string channelName = _params[2];

	channelIter chanit = doesChannelExist(channelName);
    if (chanit == _channels.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + client.getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
	if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTONCHANNEL) + " " + client.getNick() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }
    if (!chanit->isUserOperator(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_CHANOPRIVSNEEDED) + " " + client.getNick() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    if (doesUserExit(userToInvite) == _clients.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + " " + client.getNick() + " " + userToInvite + " :No such nick\r\n");
        return;
    }

    if (chanit->isUserInChannel(userToInvite)) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_USERONCHANNEL) + " "+ client.getNick() + " " + userToInvite + " " + channelName + " :is already on channel\r\n");
        return;
    }
	chanit->joinUser(userToInvite);
	std::string kickMessage = client.identifier()  + " INVITE " + userToInvite + " :" + channelName + "\r\n";
	broadcastMsg(client, kickMessage, *chanit);
}

void Server::topic(Client& client)
{
    if (!client.isConnected()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick() + " :You have not registered\r\n");
        return;
    }

    if (_params.size() < 2) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string channelName = _params[1];
    channelIter chanit = doesChannelExist(channelName);
    if (chanit == _channels.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + " " + client.getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTONCHANNEL) + " " + client.getNick() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // If a topic is provided, set the new topic
    if (_params.size() > 2) {
        // Check if the client is a channel operator if the channel mode is +t
        if (chanit->hasMode('t') || !chanit->isUserOperator(client.getNick())) {
            reply(client, ":ft_irc.1337.ma " + itos(ERR_CHANOPRIVSNEEDED) + " " + client.getNick() + " :You're not channel operator\r\n");
            return;
        }

        std::string newTopic = _params[2];

        chanit->setTopic(newTopic);
        chanit->setHasTopic(true);
        std::string msg = client.identifier() + " TOPIC " + channelName + " :" + newTopic;
        broadcastMsg(client, msg, *chanit);
    }
	else {
        // If no topic is provided, return the current topic
        std::string currentTopic = chanit->getTopic();
        if (currentTopic.empty()) {
            reply(client, ":ft_irc.1337.ma " + itos(RPL_NOTOPIC) + " " + client.getNick() + " " + channelName + " :No topic is set\r\n");
        }
		else {
            reply(client, ":ft_irc.1337.ma " + itos(RPL_TOPIC) + " " + client.getNick() + " " + channelName + " :" + currentTopic + "\r\n");
        }
    }
}

