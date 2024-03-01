#include "unp.h"

int main(int ac, char** av)
{
	if (ac != 3) exit(0);

	// create an end-point communication socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Error ... \n");
		exit(1);
	}

	// fill in an Internet socket address structure with the server's IP address and port number. this will be used with connect()
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));	// set the entire structure to 0 using bzero
	addr.sin_family = AF_INET;		// set the address family to AF_INET
	
	addr.sin_port = htons(atoi(av[2]));	// set the port number and the IP address 
								// htons() translates a short integer from host byte order to network byte order

	// A struct in_addr (for ipV4 or in6_addr for ipV6) address should be passed to inet_pton().
	if (inet_pton(AF_INET, av[1], &addr.sin_addr) != 1)
	{
		// The function returns 1 if the conversion succeeds, 0 if the input is not a valid IP address, and -1 if an error occurs
		printf("Error inet_pton() \n");
		exit(2);
	}
	
	// Now addr.sin_addr contains the binary form of the IP address

	// establishes a TCP connection with the server specified by the socket address structure pointed to by the second argument.

	printf("before\n");
	connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	printf("after\n");

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