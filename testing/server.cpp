#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

void	guard(int code, std::string msg)
{
	if (code < 0)
	{
		std::perror(msg.c_str());
		exit(1);
	}
}

int	main()
{
	struct sockaddr_in servaddr;
	int					servfd;
	int					enable = 1;

	guard( (servfd = socket(AF_INET, SOCK_STREAM, 0)), "socket");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	guard(bind(servfd, (struct sockaddr*) &servaddr, sizeof(servaddr)), "bind");

	guard(listen(servfd, 5), "listen");

	int connfd = accept(servfd, NULL, NULL);
	if (connfd < 0)
	{
		perror("accept");
	}
	std::cout << "client " << connfd << " connected" << std::endl;
	// close(connfd);

	char buffer[512];
	ssize_t rr;
	fcntl(connfd, F_SETFL, O_NONBLOCK);
	fcntl(0, F_SETFL, O_NONBLOCK);
	while (1)
	{
		rr = read(connfd, buffer, sizeof(buffer));
		if (rr <= 0)
		{
			// std::cout << "error read" << std::endl;
			continue;
		}
		buffer[rr] = 0;
		std::cout << "client: " << buffer << std::endl;
		rr = read(0, buffer, sizeof(buffer));
		if (rr <= 0)
		{
			// std::cout << "error read" << std::endl;
			continue;
		}
		buffer[rr] = 0;
		write(connfd, buffer, strlen(buffer));
	}
}