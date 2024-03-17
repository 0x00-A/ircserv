#include "./inc/ircserv.hpp"

#include <iostream>
#include <cstring>
#include <netdb.h> // For getaddrinfo
#include <ctime>

int main() {

    std::time_t res = time(NULL);
    std::cout << res << std::endl;
    
    return 0;
}
