#include "Socket.hpp"
#include "Server.hpp"

int	main(int ac, char** av)
{
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
