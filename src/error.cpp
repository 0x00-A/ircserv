#include "error.hpp"

void	err(std::string msg, int errnoflag)
{
	if (errnoflag)
		std::perror(msg.c_str());
	else
		std::cerr << msg << std::endl;
}

// print error and terminate
void	errQuit(std::string msg, int errnoflag)
{
	err(msg, errnoflag);
	exit(1);
}

// print error and return
int		errRet(std::string msg, int errnoflag)
{
	err(msg, errnoflag);
	return (-1);
}
