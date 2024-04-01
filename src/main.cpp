#include "Socket.hpp"
#include "Server.hpp"

void	signalhandler(int sig)
{
	if (sig == SIGINT)
		isServerUp = false;
}

int	main(int ac, char** av)
{
	signal(SIGINT, signalhandler);
	if (ac == 3)
	{
		try
		{
			Server	irc(av[1], av[2]);
	
			irc.run();
		}
		catch ( std::exception & e )
		{
			std::cout << e.what() << std::endl;
		}
	}
	return (0);
}
