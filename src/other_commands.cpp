#include "Server.hpp"
#include "Channel.hpp"

void Server::notice(Client &client)
{
    string              response;
    clientIter          cliIt;

    if (_params.size() < 3)
        return ;
    if ( (cliIt = doesUserExit(_params[1])) != _clients.end())
    {
        response = client.identifier() + " NOTICE " + cliIt->getNick() + " :" +  _params[2];
        reply(*cliIt, response);
    }
}

void Server::names(Client &client)
{
    channelIter         chanIt;
    string              response;

    if (_params.size() >= 2)
    {
        if ( (chanIt = doesChannelExist(_params[1])) != _channels.end())
        {
            showChannelNames(client, *chanIt);
        }
    }
    else
    {
        for (size_t i = 0; i < _channels.size() ; i++)
        {
            showChannelNames(client, _channels[i]);
        }
    }
}

void Server::lusers(Client &client) 
{
    string message;

    message = "253 " + client.getNick() + " " + itos(getUnknownConnections()) + " :unknown connection(s)";
    reply(client, message);
    message = "254 " + client.getNick() + " " + itos(getExistingChannels()) + " :channels formed";
    reply(client, message);
    message = "255 " + client.getNick() + " :" + "I have " + itos(getKnownConnections()) + " clients and 1 servers";
    reply(client, message);
}

void	Server::motd(Client& client)
{
    std::ifstream   motdfile("./data/motd.txt");
    string          line;

    if (!motdfile.is_open())
    {
        throw (_servname + " " + ERR_NOMOTD + " " + client.getNick() + " :MOTD File is missing");
    }
	reply(client, _servname + " " + RPL_MOTDSTART + " " + client.getNick() + " :- " + _servname + " Message of the Day -");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :-                                                      ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :-   __  _         _               _  _____ _____ _____ ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :-  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :- | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :- |  _|| |_      | || |  | (__   | | ___) |___) | / /  ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :- |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :-          |_____|                                     ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :-                                                      ");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :- irc1337 is a really cool network!");
	// reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " :- No spamming please, thank you!");
    while (std::getline(motdfile, line))
    {
        reply(client, _servname + " " + RPL_MOTD + " " + client.getNick() + " " + line);
    }
	reply(client, _servname + " " + RPL_ENDOFMOTD + " " + client.getNick() + " :End of /MOTD command.");
}
