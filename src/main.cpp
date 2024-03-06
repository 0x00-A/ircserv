#include "TCPSocket.hpp"
#include "IRCServer.hpp"

int	main(int ac, char** av)
{
	if (ac == 3)
	{
		TCPSocket	sock(av[1]);
		IRCServer	ircserv(av[1], av[2], sock.getfd());

		ircserv.start();
		sock.close();
	}
	return (0);
}
