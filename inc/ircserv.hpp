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
# include <cerrno>

using std::string;
using std::cout;
using std::cerr;
using std::perror;
using std::endl;
using std::stringstream;


std::string itos(int num);

// struct unordered {
//     bool operator()(const string& a, const string& b) const {
// 		(void)a;
// 		(void)b;
//         return true;
//     }
// };

// #define std::set<string> std::set<string, unordered>

#endif //IRCSERV_HPP
