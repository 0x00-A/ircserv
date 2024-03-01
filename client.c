#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 512

int	main(int ac, char** av)
{
	struct sockaddr_in	addr;
	int					sockfd;
	ssize_t				readed;
	char				buffer[MAX_LEN];

	if (ac != 3) exit(5);

	// fill in a network socket address struct
	addr.sin_family = AF_INET6;
	addr.sin_port = htons(atoi(av[2]));
	if (inet_pton(AF_INET, av[1], &addr.sin_addr) <= 0)
	{
		printf("inet_pton failed\n");
		exit(1);
	}
	// create a socket
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket() failed\n");
		exit(2);
	}
	// establish a tcp connection to the server specified in struct
	printf("1\n");
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("connect failed\n");
		strerror(errno);
		exit(3);
	}
	printf("2\n");
	while ( (readed = read(sockfd, buffer, MAX_LEN - 1)) > 0)
	{
		buffer[readed] = 0;
		// write(1, buffer, strlen(buffer));
		fputs(buffer, stdout);
	}
	if (readed < 0)
	{
		printf("read failed\n");
		exit(4);
	}
	exit(0);
}