#include "Socket.hpp"
#include "Server.hpp"

int	main(int ac, char** av)
{
	if (ac == 3)
	{
		Socket	sock(av[1]);
		Server	ircserv(av[1], av[2], sock.getfd());

		ircserv.start();
		sock.close();
	}
	return (0);
}
