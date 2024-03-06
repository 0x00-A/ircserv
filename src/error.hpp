#ifndef ERROR_HPP
# define ERROR_HPP

# include <cerrno>
# include <string>
# include <cstdlib>
# include <iostream>
# include <cstdio>
# include <cerrno>

void		errQuit(std::string msg, int errnoflag);
int			errRet(std::string msg, int errnoflag);

#endif
