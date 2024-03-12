#include "Socket.hpp"
#include "Server.hpp"

int	main(int ac, char** av)
{
	if (ac == 3)
	{
		Server	ircserv(av[1], av[2]);

		ircserv.start();
	}
	return (0);
}
