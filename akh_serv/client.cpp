#include <cstdlib>
#include <sys/types.h> // Include for data types
#include <sys/socket.h> // Include for socket functions
#include <netinet/in.h> // Include for internet domain addresses
#include <netdb.h> // Include for network database operations
#include <unistd.h> // Include for POSIX operating system API
#include <cstring> // Include for memory manipulation
#include <iostream> // Include for input and output stream

void error(const char *msg) {
    std::cerr << msg << std::endl; // Print error message and exit
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno = 12345; // Socket descriptor and port number
    struct sockaddr_in serv_addr; // Structure for server address
    struct hostent *server; // Structure to store server information

    if (argc < 2) {
       std::cerr << "usage " << argv[0] << " hostname" << std::endl; // Check for hostname argument
       exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]); // Resolve hostname to IP
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl; // Check if the hostname is resolved
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr)); // Clear structure
    serv_addr.sin_family = AF_INET; // Set address family to Internet
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length); // Copy server address
    serv_addr.sin_port = htons(portno); // Convert port number to network byte order

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting"); // Connect to the server

    std::cout << "Connected to the server. Type your message and press enter. To exit, press Ctrl+D.\n";
    
    while (true) {
        char buffer[256];
        std::cout << "> ";
        std::cin.getline(buffer, 256); // Read input from the user
        if (std::cin.eof()) { // Check for EOF (Ctrl+D)
            break; // Exit the loop if EOF is detected
        }

        write(sockfd, buffer, strlen(buffer)); // Send the input to the server
        memset(buffer, 0, 256); // Clear the buffer
        // Optionally, wait for and read a response from the server
        read(sockfd, buffer, 255); // Read the server's response (optional)
        std::cout << "Server reply: " << buffer << std::endl; // Print the server's response
        memset(buffer, 0, 256); // Clear the buffer again
        std::cout << "buffer: " << buffer << '\n'
    }

    close(sockfd); // Close the socket
    std::cout << "Connection closed." << std::endl;
    return 0; // End the program
}


