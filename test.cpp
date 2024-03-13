#include "./inc/ircserv.hpp"

#include <iostream>
#include <cstring>
#include <netdb.h> // For getaddrinfo

int main() {
    // const char* ipToResolve = "www.google.com"; // Example IP address

    // struct addrinfo hints = {0};
    // hints.ai_family = AF_INET; // Allow both IPv4 and IPv6
    // hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    // hints.ai_flags = AI_CANONNAME;
    // struct addrinfo* result;
    // int status = getaddrinfo(ipToResolve, nullptr, &hints, &result);
    // if (status == 0) {
    //     // 'result' now contains information about the IP address
    //     std::cout << "Resolved IP address: " << ipToResolve << std::endl;
    //     if (result->ai_canonname)
    //         std::cout << "Hostname: " << result->ai_canonname << std::endl;
    //     freeaddrinfo(result); // Clean up
    // } else {
    //     std::cerr << "Hostname resolution failed." << std::endl;
    // }

    string    s = "-okl";

    // s.erase(2, 1);
    s.erase(s.find("k"), 1);
    std::cout << s << std::endl;
    return 0;
}
