#include "unp.h"

int main()
{
	// create an end-point communication socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		strerror(errno);
		printf("Helo\n");
		exit(1);
	}

	// fill in an Internet socket address structure with the server's IP address and port number.
	struct sockaddr_in addr;
	// set the entire structure to 0 using bzero
	bzero(&addr, sizeof(addr));
	// set the address family to AF_INET
	addr.sin_family = AF_INET;
	// set the port number and the IP address
	addr.sin_port = htons(37); // translates a short integer from host byte order to network byte order

	// create a struct in_addr (for ipV4 and in6_addr for ipV6) and pass its address to the function.
	// struct in_addr ip_bin;
	// bzero(&ip_bin, sizeof(struct in_addr));
	if (inet_pton(AF_INET, "129.6.15.29", &addr.sin_addr) != 1)
	{
		// The function returns 1 if the conversion succeeds, 0 if the input is not a valid IP address, and -1 if an error occurs
		// strerror(errno);
		printf("Error inet_pton() \n");
		exit(1);
	}
	
	// Now ip_bin contains the binary form of the IP address
    // and can be used with other socket functions
	// addr.sin_addr.s_addr = ip_bin.s_addr;

	// establishes a TCP connection with the server specified by the socket address structure pointed to by the second argument.

	connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));

	//////////////////////////////////////////////////////////////////////////////////////////////
	//	Everytime one of the socket functions requires a pointer to a socket address structure,	//
	//	that pointer must be cast to a pointer to a generic socket address structure. This is 	//
	//	because the socket functions predate the ANSI C standard, so the void * pointer type	//
	//	was not available in the early 1980s when these functions were developed.				//
	//////////////////////////////////////////////////////////////////////////////////////////////


	char buffer[512];
	int n;
	while ((n = read(sockfd, buffer, 511)) > 0)
	{
		buffer[n] = 0;
		// printf("%d\n", n);
		// printf("%s\n", buffer);
		fputs(buffer, stdout);
	}
	if (n < 0)
		printf("Error\n");
}