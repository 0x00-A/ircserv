# include "IRCbot.hpp"

// void signal_handler(int sig)
// {
// 	if (sig == SIGINT || sig == SIGQUIT)
// 		IsBotRunning = false;
// }

int main(int ac, char** av)
{
	if (ac != 5)
		return (0);
	// signal(SIGINT, signal_handler);
	// signal(SIGQUIT, signal_handler);
	IRCbot	bot(av[1], av[2], av[3], av[4]);
	try
	{
		bot.connectToIRCServer();
		bot.run();
	}
	catch ( string & msg )
	{
		std::cout << msg << std::endl;
	}

	return 0;
}
