#include "unp.h"

int	main()
{
	// creation of the TCP socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(13);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	The server's port (13 for the daytime service) is bound to							//
	//	the socket by filling in an Internet socket address structure and calling			 //
	//	bind. We specify the IP address as INADDR_ANY, which allows the server				//
	//	to accept a client connection on any interface										//
	//////////////////////////////////////////////////////////////////////////////////////////

	if (bind(listenfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		printf("bind failed ... \n");
		exit(1);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//	By calling listen, the socket is converted into a listening socket, on 				//
	//	which incoming connections from clients will be accepted by the kernel. These		//
	//	three steps, socket, bind, and listen, are the normal steps for any TCP				//
	//	server to prepare what we call the listening descriptor								//
	//////////////////////////////////////////////////////////////////////////////////////////
	
	if (listen(listenfd, 2) < 0)
	{
		printf("listen failed ... \n");
		exit(1);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//	Normally, the server process is put to sleep in the call to accept, waiting			//
	//	for a client connection to arrive and be accepted. A TCP connection uses what		//
	//	is called a three-way handshake to establish a connection. When this handshake		//
	//	completes, accept returns, and the return value from the function is a new			//
	//	descriptor (connfd) that is called the connected descriptor. This new descriptor	//
	//	is used for communication with the new client. A new descriptor is returned by		//
	//	accept for each client that connects to our server.									//
	//////////////////////////////////////////////////////////////////////////////////////////

	socklen_t len = sizeof(addr);
	for( ; ; )
	{
		char buf[1000];
		int connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

		snprintf(buf, sizeof(buf),  "hello");	// A carriage return and linefeed are appended to the string by snprintf
		write(connfd, buf, strlen(buf));

		close(connfd);

		//////////////////////////////////////////////////////////////////////////////////////////
		//	The server closes its connection with the client by calling close. This initiates	//
		//	the normal TCP connection termination sequence: a FIN is sent in each direction and	//
		//	each FIN is acknowledged by the other end.	(more on Section 2.6)					//
		//////////////////////////////////////////////////////////////////////////////////////////
	}

		/*	As with the client, the server is protocol-dependent on IPv4. We will show a
			protocol-independent version that uses the getaddrinfo function in Figure 11.13. */

		/*	Our server handles only one client at a time. If multiple client connections arrive
			at about the same time, the kernel queues them, up to some limit, and returns
			them to accept one at a time. */

		/*	This server is called an iterative server because it iterates through each client,
			one at a time.  */

		/*	If we start a server like this from a shell command line, we might want the
			server to run for a long time, since servers often run for as long as the system
			is up. This requires that we add code to the server to run correctly as a Unix
			daemon: a process that can run in the background, unattached to a terminal. We
			will cover this in Section 13.4. */

		exit(0);
}