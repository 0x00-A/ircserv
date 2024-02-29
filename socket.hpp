#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

class Socket {
private:
    int sockfd;
    struct sockaddr_in servaddr, cli;

public:
    Socket(char* host, int port, bool isServer) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("Socket creation failed...\n");
            exit(0);
        } else {
            printf("Socket successfully created..\n");
        }
        bzero(&servaddr, sizeof(servaddr));

        // Assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = isServer ? htonl(INADDR_ANY) : inet_addr(host);
        servaddr.sin_port = htons(port);

        if (isServer) {
            // Binding newly created socket to given IP and verification
            if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
                printf("Socket bind failed...\n");
                exit(0);
            } else {
                printf("Socket successfully binded..\n");
            }
        } else {
            // Connect the client socket to server socket
            if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
                printf("Connection with the server failed...\n");
                exit(0);
            } else {
                printf("Connected to the server..\n");
            }
        }
    }

    void listenSocket() {
        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        } else {
            printf("Server listening..\n");
        }
    }

    int acceptConnection() {
        socklen_t len = sizeof(cli);

        // Accept the data packet from client and verification
        int connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } else {
            printf("Server accepted the client..\n");
        }

        return connfd;
    }

    void closeSocket() {
        close(sockfd);
    }
};
