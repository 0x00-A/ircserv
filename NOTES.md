+ it is possible for an application to bypass the transport layer and use IPv4 or IPv6 directly. This is called a raw socket
+ we can even bypass the IP layer completely to read and write our own datalink-layer frames.
+ The sockets programming interfaces described in this book are interfaces from the upper three layers (the "application") 
into the transport layer. This is the focus of this book: how to write applications using sockets that use either TCP or UDP


+ TCP provides flow control. TCP always tells its peer exactly how many bytes of data it is willing to accept from the peer at 
any one time. This is called the advertised window. At any time, the window is the amount of room currently available in the 
receive buffer, guaranteeing that the sender cannot overflow the receive buffer. The window changes dynamically over time: As 
data is received from the sender, the window size decreases, but as the receiving application reads data from the buffer, the
window size increases. It is possible for the window to reach 0: when TCP's receive buffer for a socket is full and it must 
wait for the application to read data from the buffer before it can take any more data from the peer.


+ Finally, a TCP connection is full-duplex. This means that an application can send and receive data in both directions on
a given connection at any time. This means that TCP must keep track of state information such as sequence numbers and window
sizes for each direction of data flow: sending and receiving. After a full-duplex connection is established, it can be turned
into a simplex connection if desired

+ When a client wants to contact a server, the client must identify the server with which it wants to communicate. TCP, UDP, 
and SCTP define a group of well-known ports to identify well-known services. For example, every TCP/IP implementation that 
supports FTP assigns the well-known port of 21 (decimal) to the FTP server. Trivial File Transfer Protocol (TFTP) servers 
are assigned the UDP port of 69.


+ Clients, on the other hand, normally use ephemeral ports, that is, short-lived ports. These port numbers are normally 
assigned automatically by the transport protocol to the client. Clients normally do not care about the value of the ephemeral 
port; the client just needs to be certain that the ephemeral port is unique on the client host. The transport protocol code 
guarantees this uniqueness.


+ The dynamic or private ports, 49152 through 65535. The IANA says nothing about these ports. These are what we call ephemeral ports.

+ The socket pair for a TCP connection is the four-tuple that defines the two endpoints of the connection: the local IP address, 
local port, foreign IP address, and foreign port. A socket pair uniquely identifies every TCP connection on a network.

+ bind lets the application specify the local IP address and local port for TCP

+ The two values that identify each endpoint, an IP address and a port number, are often called a socket.

+ socket functions that pass a socket address structure from the process to the kernel, bind, connect

+ sin.len: The four socket functions that pass a socket address structure from the process to the kernel, bind, connect, sendto, and 
sendmsg, all go through the sockargs function in a Berkeley-derived implementation (p. 452 of TCPv2). This function copies the socket 
address structure from the process and explicitly sets its sin_len member to the size of the structure that was passed as an argument 
to these four functions. The five socket functions that pass a socket address structure from the kernel to the process, accept, recvfrom, 
recvmsg, getpeername, and getsockname, all set the sin_len member before returning to the process.

+ sin_zero: Almost all implementations add the sin_zero member so that all socket address structures are at least 16 bytes in size.

+ Since the kernel is passed both the pointer and the size of what the pointer points to, it knows exactly how much data to copy from 
the process into the kernel

+ The reason that the size changes from an integer to be a pointer to an integer is because the size is both a value when the function 
is called (it tells the kernel the size of the structure so that the kernel does not write past the end of the structure when filling 
it in) and a result when the function returns (it tells the process how much information the kernel actually stored in the structure). 
This type of argument is called a value-result argument.

+ error EINTR (the system call was interrupted by a caught signal)

+ Socket address structures are self-defining because they always begin with a field (the "family") that identifies the address family 
contained in the structure. Newer implementations that support variable-length socket address structures also contain a length field 
at the beginning, which contains the length of the entire structure.

+ The client does not have to call [[bind]] (which we will describe in the next section) 
before calling connect: the kernel will choose both an ephemeral port and the source 
IP address if necessary.
-> Client Connection Process:
	+ When a client application establishes a connection to a server using the connect system call, it does not necessarily need to call bind explicitly before that.
	+ The kernel automatically selects an ephemeral port (a temporary port) and the source IP address for the client.
	+ The ephemeral port is used for the client-side of the connection, allowing multiple clients to connect to the same server on different ports.
	+ The source IP address is typically the IP address of the client machine.

+ In the case of a TCP socket, the [[connect]] function initiates TCP's three-way handshake 
(Section 2.6). The function returns only when the connection is established or an 
error occurs.

+ The [[bind]] function assigns a local protocol address to a socket. With the Internet 
protocols, the protocol address is the combination of either a 32-bit IPv4 address or 
a 128-bit IPv6 address, along with a 16-bit TCP or UDP port number.

+  When a socket is created by the socket function, it is assumed to be an active 
socket, that is, a client socket that will issue a connect. The [[listen]] function 
converts an unconnected socket into a passive socket, indicating that the 
kernel should accept incoming connection requests directed to this socket. In 
terms of the TCP state transition diagram (Figure 2.4), the call to listen
moves the socket from the CLOSED state to the LISTEN state.
This function is normally called after both the socket and bind functions and must be 
called before calling the accept function.

+ [[accept]] is called by a TCP server to return the next completed connection from the 
front of the completed connection queue (Figure 4.7). If the completed connection 
queue is empty, the process is put to sleep (assuming the default of a blocking 
socket).


+  

=>> page 95

>> nonblocking sockets in Chapter 16










CPU_VENDOR_OS