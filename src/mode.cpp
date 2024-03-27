#include "Server.hpp"
#include "Channel.hpp"

void Server::mode(Client& client){

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
    std::string mode = _params[2];
    std::string nick = _params[3];
    if (doesUserExit(nick) == _clients.end()){
        reply(client, ":ft_irc.1337.ma " + itos(ERR_NOSUCHNICK) + " " + client.getNick() + " " + nick + " :No such nick\r\n");
        return;
    }
    if (!chanit->isUserInChannel(nick)) {
        reply(client, ":ft_irc.1337.ma " + itos(ERR_USERNOTINCHANNEL) + " " + client.getNick() + " " + channelName + " :they are not on that channel\r\n");
        return;
    }

    switch (mode[0]) {
        case '+':
            switch (mode[1]) {
                case 'i':
                    chanit->setHasInvite(true);
                    break;
                case 'l':
                    // int limit = std::atoi(_params[3].c_str());
                    chanit->setUserLimit(_params[3]);
                    break;
                case 'k':
                    chanit->setPasskey(nick);
                    break;
                case 't':
                    chanit->setHasTopic(true);
                    break;
                case 'o':
                    chanit->setChannelOperator(nick);
                    broadcastMsg(client, clien)
                    break;
                default:
                    reply(client, ":ft_irc.1337.ma " + itos(ERR_UNKNOWNMODE) + " " + client.getNick() + " " + mode + " :Unknown mode\r\n");
                    return;
            }
            break;
        case '-':
            switch (mode[1]) {
                case 'i':
                    chanit->setHasInvite(false);
                    break;
                case 'l':
                    chanit->setHasUserLimit(false);
                    break;
                case 'k':
                    chanit->setHasPasskey(false);
                    break;
                case 't':
                    chanit->setHasTopic(false);
                    break;
                case 'o':
                    chanit->setChannelOperator(nick);
                    break;
                default:
                    reply(client, ":ft_irc.1337.ma " + itos(ERR_UNKNOWNMODE) + " " + client.getNick() + " " + mode + " :Unknown mode\r\n");
                    return;
            }
            break;
        default:
            reply(client, ":ft_irc.1337.ma " + itos(ERR_UNKNOWNMODE) + " " + client.getNick() + " " + mode + " :Unknown mode\r\n");
            return;
    }
}