# include "ircbot.hpp"

int main(int ac, char** av)
{
	if (ac != 4)
		return (0);

	ircbot	banbot(av[1], av[2], av[3]);
	try
	{
		banbot.connectToServer();
		banbot.run();
	}
	catch ( ... )
	{
		std::cout << "what" << std::endl;
	}

	return 0;
}