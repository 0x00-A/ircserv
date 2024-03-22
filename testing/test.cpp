#include <fcntl.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

int main()
{
	// int sockFd = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket

	// // Get the current flags
	// int flags = fcntl(sockFd, F_GETFL, 0);
	// if (flags == -1) {
	// 	// Handle error
	// }

	// if (!flags)
	// {
	// 	std::cout << "empty" << std::endl;
	// }
	// else
	// {
	// 	std::cout << flags << std::endl;
	// 	std::cout << "not empty" << std::endl;
	// }

	// // Modify the flags as needed
	// flags |= O_NONBLOCK;  // Add the O_NONBLOCK flag

	// // Set the new flags
	// int status = fcntl(sockFd, F_SETFL, flags);
	// if (status == -1) {
	// 	// Handle error
	// }
	int rc = write(4, "hi", 2);
	std::cout << rc << std::endl;
	if (errno == EPIPE)
		std::cout << "YES" << std::endl;
	while (1)
		;
}

