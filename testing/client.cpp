#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

void	sighandler(int sig)
{
	if (sig == SIGPIPE)
		std::cout << "SIGPIPE caught" << std::endl;
}

int	main()
{
	int clifd, rc;

	clifd = socket(AF_INET, SOCK_STREAM, 0);
	if (clifd == -1)
	{
		std::perror("socket");
		exit(1);
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);

	rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	if (rc == -1)
	{
		std::cout << "inet_pton failed" << std::endl;
		close(clifd);
		exit(1);
	}

	// rc = bind(clifd, (struct sockaddr *) &addr, sizeof(addr));
	// if (rc == -1)
	// {
	// 	std::perror("bind");
	// 	exit(1);
	// }

	signal(SIGPIPE, sighandler);
	signal(SIGQUIT, sighandler);
	rc = connect(clifd, (struct sockaddr *) &addr, sizeof(addr));
	if (rc == -1)
	{
		std::perror("connect");
		close(clifd);
		exit(1);
	}

	char buffer[512];
	ssize_t		rr;

	fcntl(clifd, F_SETFL, O_NONBLOCK);
	fcntl(0, F_SETFL, O_NONBLOCK);
	while (1)
	{
		rr = read(clifd, buffer, sizeof(buffer));
		if (rr < 0)
		{
			// std::cout << "read error" << std::endl;
			continue;;
		}
		if (rr == 0)
		{
			std::cout << "end of connection" << std::endl;
			exit(1);
		}
		buffer[rr] = 0;
		std::cout << "server: " << buffer << std::endl;
		rr = read(0, buffer, sizeof(buffer));
		if (rr <= 0)
		{
			// std::cout << "read error" << std::endl;
			continue;
		}
		buffer[rr] = 0;
		write(clifd, buffer, strlen(buffer));
	}
}