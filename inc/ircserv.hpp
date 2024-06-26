#ifndef IRCSERV_HPP
#define IRCSERV_HPP

# include <string>
# include <vector>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <map>
# include <sstream>
# include <iomanip>
# include <set>
# include <queue>
# include <exception>
# include <cstdio>
# include <string>
# include <string.h>
# include <cstdlib>
# include <cctype>
# include <cerrno>
# include <signal.h>
# include <algorithm>
# include <fstream>
# include "numReplies.hpp"
# include <ctime>

using std::string;
using std::cout;
using std::cerr;
using std::perror;
using std::endl;
using std::stringstream;

std::string		itos(int num);
struct tm		*getCurrentTime();
bool			isdigitstring(const string& str);
void            to_upper(string& str);

#endif //IRCSERV_HPP
