#include "Socket.hpp"
#include "Server.hpp"

void	sighandler(int sig)
{
	cout << sig << " SIGPIPE recieved\n";
}

int	main(int ac, char** av)
{
	signal(SIGPIPE, sighandler);
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
