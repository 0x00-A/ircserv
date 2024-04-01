#include "Server.hpp"

void	motd(Client& client)
{
	/*
	C -> MOTD
	S <- :irc.example.com 375 chris :- irc.example.com Message of the Day -
	S <- :irc.example.com 372 chris :- ExampleNet is a really cool network!
	S <- :irc.example.com 372 chris :- No spamming please, thank you!
	S <- :irc.example.com 376 chris :End of /MOTD command.
	*/
}