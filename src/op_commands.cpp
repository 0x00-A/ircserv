#include "Server.hpp"
#include "Channel.hpp"

void Server::kick(Client& client)
{
    if (!client.isConnected()) {
        reply(client, _servname + " " + ERR_NOTREGISTERED + " " + client.getNick() + " :You have not registered");
        return;
    }
    if (_params.size() < 3) {
        reply(client, _servname + " " + ERR_NEEDMOREPARAMS + " " + client.getNick() + " KICK :Not enough parameters");
        return;
    }

    std::string channelName = _params[1];
    std::string key = _params[2];

	channelIter chanit = doesChannelExist(channelName);
    if (chanit == _channels.end()) {
        reply(client, _servname + " " + ERR_NOSUCHCHANNEL + " " + client.getNick() + " " + channelName + " :No such channel");
        return;
    }
    if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, _servname + " " + ERR_NOTONCHANNEL + " " + client.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    if (!chanit->isUserOperator(client.getNick())) {
        reply(client, _servname + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNick() + " " + channelName + " :You're not a channel operator");
        return;
    }

	if (doesUserExit(key) == _clients.end()) {
        reply(client, _servname + " " + ERR_NOSUCHNICK + " " + client.getNick() + " " +  key + " :No such nick/channel");
        return;
	}
    if (!chanit->isUserInChannel(key)) {
        reply(client, _servname + " " + ERR_USERNOTINCHANNEL + " " + client.getNick() + " " + key + " " + channelName + " :they aren't on that channel");
        return;
    }
    std::string kickMessage;

    if (_params.size() > 3)
        kickMessage = client.identifier() + " KICK " + channelName + " " + key + " :" + _params[3];
    else
        kickMessage = client.identifier() + " KICK " + channelName + " " + key + " :" + key;

    reply(client, kickMessage);
	broadcastMsg(client, kickMessage, *chanit);
	removeUserFromChannel(key, channelName);
}

void Server::invite(Client& client)
{
    clientIter invitedUserIter;


    if (!client.isConnected()) {
        reply(client, "451 :You have not registered");
        return;
    }

    if (_params.size() == 1)
    {
        std::vector<string> invitedChannels;
        string              iChannels;

        invitedChannels = client.getInvitedChannels();
        if (!invitedChannels.empty())
        {
            for (size_t i = 0; i < invitedChannels.size(); i++)
            {
                iChannels += invitedChannels[i] + " ";
            }
            reply(client, "336 " + client.getNick() + " :" + iChannels);
        }
        reply(client, "337 " + client.getNick() + " :End of INVITE list.");
        return ;

    }
    if (_params.size() < 3) {
        reply(client, "461 " + client.getNick() + " INVITE :Not enough parameters");
        return;
    }
    std::string invitedUser = _params[1];
    std::string chanName = _params[2];

	channelIter chanit = doesChannelExist(chanName);
    if (chanit == _channels.end()) {
        throw (_servname + " " + ERR_NOSUCHCHANNEL + client.getNick() + " " + chanName + \
                " :No such channel");
    }
	if (!chanit->isUserInChannel(client.getNick())) {
        throw (_servname + " " + ERR_NOTONCHANNEL + " " + client.getNick() + " " + chanName + \
                " :You're not on that channel");
    }
    if (!chanit->isUserOperator(client.getNick())) {
        throw (_servname + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNick() + " " + chanName + \
                " :You're not a channel operator");
    }
    if ( (invitedUserIter = doesUserExit(invitedUser)) == _clients.end()){
        throw (_servname + " " + ERR_NOSUCHNICK + " " + client.getNick() + " " + invitedUser + \
                " :No such nick/channel");
    }
    if (chanit->isUserInChannel(invitedUser)) {
        throw (_servname + " " + ERR_USERONCHANNEL + " "+ client.getNick() + " " + invitedUser + " " + \
                chanName + " :is already on channel");
    }
    reply(client, _servname + " " + RPL_INVITING + " " + client.getNick() + " " + invitedUser + " " + chanName);
    reply(*invitedUserIter, client.identifier() + " INVITE " + invitedUser + " :" + chanName);
    invitedUserIter->inviteToChannel(chanName);
}

void Server::topic(Client& client)
{
    if (!client.isConnected()) {
        reply(client, _servname + " " + ERR_NOTREGISTERED + " " + client.getNick() + " :You have not registered");
        return;
    }
    if (_params.size() < 2) {
        reply(client, _servname + " " + ERR_NEEDMOREPARAMS + " " + client.getNick() + " TOPIC :Not enough parameters");
        return;
    }
    std::string channelName = _params[1];
    channelIter chanit = doesChannelExist(channelName);
    if (chanit == _channels.end()) {
        reply(client, _servname + " " + ERR_NOSUCHCHANNEL + " " + client.getNick() + " " + channelName + " :No such channel");
        return ;
    }
    if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, _servname + " " + ERR_NOTONCHANNEL + " " + client.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    if(_params.size() > 2)
    {
        if (chanit->hasTopic() && !chanit->isUserOperator(client.getNick())) {
            reply(client, _servname + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNick()  + " "  + channelName + " :You're not channel operator");
            return;
        }
        std::string newTopic = _params[2];
        cout << "newTopic: " << newTopic << endl;
        if (newTopic.size() >= TOPICLEN)
            newTopic.erase(TOPICLEN);
        chanit->setTopic(newTopic);
        chanit->setTimeOfTopic();
        std::string msg = client.identifier() + " TOPIC " + channelName + " :" + newTopic;
        reply(client, msg);
        broadcastMsg(client, msg, *chanit);
    }
	else
    {
        std::string currentTopic = chanit->getTopic();
        if (currentTopic.empty())
            reply(client, _servname + " " + RPL_NOTOPIC + " " + client.getNick() + " " + channelName + " :No topic is set.");
		else
        {
            reply(client, _servname + " " + RPL_TOPIC + " " + client.getNick() + " " + channelName + " :" + currentTopic);
            reply(client, _servname + " " + RPL_TOPICWHOTIME + " " + client.getNick() + " " + channelName + " "  + client.identifier() + " " + itos(chanit->getTimeOfTopic()));
        }
        return;
    }
}

void    Server::mode(Client& client)
{
    std::queue<std::pair<string, string> >      modes;
    channelIter                                 chan;
    string                                      paramsave;
    string                                      modesave;
    int                                         modeCount = 0;

    if (!client.isConnected())
    {
        throw (_servname + " " + ERR_NOTREGISTERED + " " + \
        client.getNick()  + " :You have not registered");
    }
    if (_params.size() < 2)
    {
        throw (_servname + " " + ERR_NEEDMOREPARAMS + " " + \
        client.getNick() + " " + _params[0]  + " :Not enough parameters");
    }
    if ( (chan = doesChannelExist(_params[1])) == _channels.end())
    {
        throw (_servname + " " + ERR_NOSUCHCHANNEL + " " + \
        client.getNick() + " " + _params[1]  + " :No such channel");
    }
    if (_params.size() == 2)
    {
        reply(client, _servname + " " + RPL_CHANNELMODEIS + " " + \
        client.getNick() + " " + _params[1] + " " + chan->channelModeIs());
        throw (_servname + " " + RPL_CREATIONTIME + " " + \
        client.getNick() + " " + _params[1] + " " + chan->getCreationTime());
    }
    parseModes(modes, client);
    if (!chan->isUserOperator(client.getNick()))
    {
        throw (_servname + " " + ERR_CHANOPRIVSNEEDED + " " + \
        client.getNick() + " " + _params[1] + " " + " :You're not channel operator");
	}
	while (!modes.empty())
	{
        if (modeCount >= MODES)
            break;
		std::pair<string, string> m = modes.front();
		switch (m.first[1])
		{
			case ('o'):
			    handleOperatorFlag(m, modesave, paramsave, chan, client);
			    break;
			case ('l'):
			    handleLimitFlag(m, modesave, paramsave, chan, client);
			    break;
			case ('k'):
			    handlePasskeyFlag(m, modesave, paramsave, chan);
			    break;
			case ('i'):
			    handleInviteFlag(m, modesave, chan);
			    break;
			case ('t'):
			    handleTopicFlag(m, modesave, chan, client);
			    break;
		}
		modes.pop();
        modeCount++;
	}
	removeExtraPlusMinus(modesave);
	if (!modesave.empty())
		throw (client.identifier() + " " + _params[0] + " " + _params[1] + " " + modesave + paramsave);
}

/********************************************************************************/
/*                                     UTILS                                    */
/********************************************************************************/

bool    Server::parseModes(std::queue< std::pair<string, string> >& modes, Client& client)
{
    string sign = "+";
    int     modeCount = 0;

    for (size_t i = 0, k = 3; i < _params[2].size(); i++)
    {
        if (modeCount >= MODES)
            break;
        if (_params[2][i] == '-' || _params[2][i] == '+')
        {
            sign = _params[2].substr(i, 1);
            continue;
        }
        char c = _params[2][i];
        if (c == 'o' || c == 'l' || c == 'k' || c == 't' || c == 'i')
        {
            if ( (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+"))
                && (!(k < _params.size()) || _params[k].find_first_not_of(" ") == string::npos))
            {
                throw ( _servname + " " + ERR_NEEDMOREPARAMS + " " + \
                client.getNick() + " " + _params[0]  + " :Not enough parametersss" );
            }
            if (c == 'k' && sign == "+")
            {
                if (badFormKey(_params[k]))
                    throw (_servname + " " + ERR_INVALIDKEY + " " + client.getNick() + " " + _params[1] + \
                            " :Key is not well-formed");
                if (doesChannelExist(_params[1])->hasPasskey())
                {
                    reply(client, _servname + " " + ERR_KEYALREADYSET + " " + client.getNick() + " " + \
                        _params[1] + " :Channel key already set");
                }
            }
            if (c == 'o' || (c == 'k' && sign == "+") || (c == 'l' && sign == "+"))
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), _params[k++]));
            else
                modes.push(std::make_pair(sign + _params[2].substr(i, 1), ""));
        }
        else
        {
            throw ( _servname + " " + ERR_UNKNOWNMODE + " " + client.getNick() + \
            " " + _params[2].substr(i, 1) + " :is an unknown mode char to me" );
        }
        modeCount++;
    }
    return (true);
}

void Server::handleOperatorFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan, Client& cli)
{
    if (doesUserExit(m.second) == _clients.end())
    {
        reply(cli, _servname + " " + ERR_NOSUCHNICK + " " + \
            cli.getNick() + " " + m.second + " :No such nick");
    }
    else if (!chan->isUserInChannel(m.second))
    {
        reply(cli, _servname + " " + ERR_USERNOTINCHANNEL + " " + \
            cli.getNick() + " " + _params[1] + " :They aren't on that channel");
    }
    else if (m.first == "+o" && chan->setChannelOperator(m.second))
    {
        broadcastMsg(cli, cli.identifier() + " MODE " + _params[1] + " " + m.first + " " + m.second, *chan);
        modesave += m.first;
        paramsave += " " + m.second;
    }
    else if (m.first == "-o" && chan->unsetChannelOperator(m.second))
    {
        broadcastMsg(cli, cli.identifier() + " MODE " + _params[1] + " " + m.first + " " + m.second, *chan);
        modesave += m.first;
        paramsave += " " + m.second;
    }
}

void Server::handleLimitFlag(strPair &m, string &modesave, string &paramsave, channelIter &chan, Client& cli)
{
	if (m.first == "+l")
	{
		if (checkValidLimit(m.second))
		{
			chan->setMode(m.first);
			chan->setHasUserLimit(true);
			chan->setUserLimit(m.second);
			modesave += m.first;
			paramsave += " " + m.second;
		}
        else
        {
            reply(cli, _servname + " " + ERR_INVALIDMODEPARAM + " " + cli.getNick() + " " + chan->getName() + \
                    " :Invalid limit mode parameter. Syntax: <limit>.");
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
        if (chan->hasPasskey())
            return;
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
            chan->setPasskey("*");
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

void Server::handleTopicFlag(strPair &m, string &modesave, channelIter &chan, Client &cli)
{
    if (m.first == "+t" && !chan->hasTopic())
    {
        chan->setMode(m.first);
        chan->setHasTopic(true);
        broadcastMsg(cli, cli.identifier() + " MODE " + _params[1] + " " + m.first, *chan);
        modesave += m.first;
        cout << chan->getName() << " MODE +t\n";
    }
    else if (m.first == "-t" && chan->hasTopic())
    {
        chan->setMode(m.first);
        chan->setHasTopic(false);
        broadcastMsg(cli, cli.identifier() + " MODE " + _params[1] + " " + m.first, *chan);
        modesave += m.first;
        cout << chan->getName() << " MODE -t\n";
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

bool Server::badFormKey(string &key)
{
    if (key.find_first_of(" ") == std::string::npos)
        return (false);
    return (true);
}

bool Server::checkValidLimit(string &s)
{
    if (!isdigitstring(s) || s.length() > 19)
        return (false);
    unsigned long long int n;
    n = std::strtoull(s.c_str(), NULL, 10);
    if (n > __LONG_MAX__)
        return (false);
    return (true);
}
