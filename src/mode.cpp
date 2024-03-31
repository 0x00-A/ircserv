#include "Server.hpp"
#include "Channel.hpp"


void Server::_Operator(std::string nick, std::string chName, Client &cli ,bool _Op)
{
    channelIter chanit = doesChannelExist(chName);
    if (doesUserExit(nick) == _clients.end()){
        reply(cli, ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + " " + cli.getNick() + " " + nick + " :No such nick\r\n");
        return;
    }
    else if (!chanit->isUserInChannel(nick)) {
        reply(cli, ":ft_irc.1337.ma " + itos(ERR_USERNOTINCHANNEL) + " " + cli.getNick() + " " + chName + " :they are not on that channel\r\n");
        return;
    }
    else {
        (_Op == true) ? chanit->setChannelOperator(nick) : chanit->unsetChannelOperator(nick);
    }
} 
void Server::mode(Client& client) {
    std::string channelName = _params[1];
    channelIter chanit = doesChannelExist(channelName);

    if (!client.isConnected()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTREGISTERED) + " " + client.getNick() + " :You have not registered\r\n");
        return;
    }

    if (_params.size() < 2) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " MODE :Not enough parameters\r\n");
        return;
    }

    if(_params.size() == 2) {
        reply(client, ":ft_irc.1337.ma " + itos(RPL_CHANNELMODEIS) + " " + client.getNick() +  " " + channelName + chanit->channelModeIs());
        return;
        
    }
    

    if (chanit == _channels.end()) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHCHANNEL) + " " + client.getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!chanit->isUserInChannel(client.getNick())) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOTONCHANNEL) + " " + client.getNick() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    std::string modeChanges = _params[2];
    bool settingMode = true;
    std::vector<std::string>::size_type paramIndex = 3;

    for (std::string::size_type i = 0; i < modeChanges.length(); ++i) {
        char mode = modeChanges[i];
        if (mode == '+') {
            settingMode = true;
            continue;
        } else if (mode == '-') {
            settingMode = false;
            continue;
        }

        switch (mode) {
            case 'i':
                chanit->setHasInvite(settingMode);
                break;
            case 'l':
                if (settingMode) {
                    if (_params.size() > paramIndex) {
                        chanit->setUserLimit(_params[paramIndex++]);
                    } else {
                        reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " :Not enough parameters for mode +l\r\n");
                        return;
                    }
                } else {
                    chanit->setHasUserLimit(false);
                }
                break;
            case 'k':
                if (settingMode) {
                    if (_params.size() > paramIndex) {
                        chanit->setPasskey(_params[paramIndex++]);
                    } else {
                        reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " :Not enough parameters for mode +k\r\n");
                        return;
                    }
                } else {
                    chanit->setHasPasskey(false);
                }
                break;
            case 't':
                chanit->setHasTopic(settingMode);
                break;
            case 'o':
                if (_params.size() > paramIndex) {
                    std::string nick = _params[paramIndex++];
                    _Operator(nick, channelName, client, settingMode);
                    std::string modeChangeMsg = client.identifier() + " MODE " + channelName + " " + (settingMode ? "+" : "-") + "o " + nick;
                    broadcastMsg(client, modeChangeMsg, *chanit);
                } else {
                    reply(client, ":ft_irc.1337.ma " + itos(ERR_NEEDMOREPARAMS) + " " + client.getNick() + " :Not enough parameters for mode +o/-o\r\n");
                    return;
                }
                break;
            default:
                reply(client, ":ft_irc.1337.ma " + itos(ERR_UNKNOWNMODE) + " " + client.getNick() + " " + mode + " :Unknown mode\r\n");
                return;
        }
    }
}