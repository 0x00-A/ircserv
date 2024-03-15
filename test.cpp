#include "./inc/ircserv.hpp"

#include <iostream>
#include <cstring>
#include <netdb.h> // For getaddrinfo

void removeExtraPlusMinus(string &s)
{
    bool sawPlus = false;
    bool sawMinus = false;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '+' && !sawPlus)
        {
            sawPlus = true;
            sawMinus = false;
            continue;
        }
        if (s[i] == '+' && sawPlus)
        {
            s.erase(i, 1);
            i--;
        }
        if (s[i] == '-' && !sawMinus)
        {
            sawMinus = true;
            sawPlus = false;
            continue;
        }
        if (s[i] == '-' && sawMinus)
        {
            s.erase(i, 1);
            i--;
        }
    }
}

int main() {

    string s = "+t+s+t-t+r+d-d-g-g";
    removeExtraPlusMinus(s);
    std::cout << s << std::endl;
    return 0;
}
