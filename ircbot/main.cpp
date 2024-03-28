# include "ircbot.hpp"

int main(int ac, char** av)
{
	if (ac != 5)
		return (0);

	ircbot	banbot(av[1], av[2], av[3], av[4]);
	try
	{
		banbot.connectToServer();
		banbot.run();
	}
	catch ( string & msg )
	{
		std::cout << msg << std::endl;
	}

	return 0;
}