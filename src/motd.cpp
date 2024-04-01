#include "Server.hpp"

void	Server::motd(Client& client)
{
	/*
	C -> MOTD
	S <- :irc.example.com 375 chris :- irc.example.com Message of the Day -
	S <- :irc.example.com 372 chris :- ExampleNet is a really cool network!
	S <- :irc.example.com 372 chris :- No spamming please, thank you!
	S <- :irc.example.com 376 chris :End of /MOTD command.
	*/
	/*
  __  _         _              _  _____ _____ _____ 
 / _|| |_      (_) _ __  ___  / ||___ /|___ /|___  |
| |_ | __|     | || '__|/ __| | |  |_ \  |_ \   / / 

|  _|| |_      | || |  | (__  | | ___) |___) | / /  
|_|   \__|_____|_||_|   \___| |_||____/|____/ /_/   
         |_____|                                    
	*/
	reply(client, RPL_MOTDSTART + " " + client.getNick() + " :- " + _servname + " Message of the Day -");
	reply(client, RPL_MOTD + " " + client.getNick() + " :                                                      ");
	reply(client, RPL_MOTD + " " + client.getNick() + " :   __  _         _               _  _____ _____ _____ ");
	reply(client, RPL_MOTD + " " + client.getNick() + " :  / _|| |_      (_) _ __  ___   / ||___ /|___ /|___  |");
	reply(client, RPL_MOTD + " " + client.getNick() + " : | |_ | __|     | || '__|/ __|  | |  |_ \\  |_ \\   / / ");
	reply(client, RPL_MOTD + " " + client.getNick() + " : |  _|| |_      | || |  | (__   | | ___) |___) | / /  ");
	reply(client, RPL_MOTD + " " + client.getNick() + " : |_|   \\__|_____|_||_|   \\___|  |_||____/|____/ /_/   ");
	reply(client, RPL_MOTD + " " + client.getNick() + " :          |_____|                                     ");
	reply(client, RPL_MOTD + " " + client.getNick() + " :                                                      ");
	reply(client, RPL_MOTD + " " + client.getNick() + " :- irc1337 is a really cool network!");
	reply(client, RPL_MOTD + " " + client.getNick() + " :- No spamming please, thank you!");
	reply(client, RPL_ENDOFMOTD + " " + client.getNick() + " :End of /MOTD command.");
}
