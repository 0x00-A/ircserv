#include <cstddef>
#include <sys/types.h> // Include for data types used in system calls
#include <sys/socket.h> // Include for socket functions
#include <netinet/in.h> // Include for internet domain address structures
#include <unistd.h> // Include for POSIX operating system API (e.g., close())
#include <cstring> // Include for memory manipulation functions (e.g., memset)
#include <iostream> // Include for input and output stream

void error(const char *msg) {
    std::cerr << msg << std::endl; // Print error message and exit
    exit(1);
}

int main() {
    int sockfd, newsockfd, portno = 12345; // Socket descriptors and port number
    socklen_t clilen; // Stores the size of the client address
    char buffer[256]; // Buffer to store messages from the client
    struct sockaddr_in serv_addr, cli_addr; // Structures for server and client addresses
    int n; // Variable to store the number of characters read or written

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create a new socket
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr)); // Clear structure
    serv_addr.sin_family = AF_INET; // Set address family to Internet
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Allow connections to any IP
    serv_addr.sin_port = htons(portno); // Convert port number to network byte order

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding"); // Bind the socket to an address

    listen(sockfd, 50000); // Listen for connections, with a max backlog of 5
    clilen = sizeof(cli_addr); // Get the size of the client address

    while(1) {
    // Accept connection
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    while(1) {
        memset(buffer, 0, 256); // Clear the buffer
        n = read(newsockfd, buffer, 255); // Read data from the socket
        if (n <= 0) break; // Break the loop if read error or client disconnects

        std::cout << "Here is the message: " << buffer << std::endl; // Print the message
    }
    close(newsockfd); // Close the client socket after done with the communication
    }

    close(sockfd); // Close the server socket
    return 0;
}
