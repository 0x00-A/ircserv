import socket

import time

clients = []

def read_from_socket(client_socket):
    # Initialize an empty response
    response = b""

    # Keep reading until no more data exists
    while True:
        # try:
            chunk = client_socket.recv(512)
            if not chunk:
                break  # No more data, exit the loop
            print(chunk.decode())
            response += chunk
        # except BlockingIOError:
            # print("ERROR NON BLOCKING IO\n")
            # recv would block, break the loop
            # pass

    # Print the received data

            # while True:
            # try:
            #     # Read data from the server
            #     data = client_socket.recv(1024)
            #     if not data:
            #         break
            #     print(data.decode(), end='')  # Print received data
            # except BlockingIOError:
            #     pass

def connect_to_server(host, port, i):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # client_socket.setblocking(False)
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = f"user x x x x\nnick lalalala{i}\n"
        client_socket.sendall(message.encode())


        # response = client_socket.recv(10000)
        # print("Received:", response.decode())

        for i in range(250):
            message = f"join #chchch{i}\n"
            client_socket.sendall(message.encode())
            print("Message sent to server:", message)
            time.sleep(0.2)
        
            # Receive data from the server
            response = client_socket.recv(4096)
            print("Received:", response.decode())

        message = f"join #notachannel,#ch2222222\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
    
        # Receive data from the server
        response = client_socket.recv(4096)
        print("Received:", response.decode())
        
        # read_from_socket(client_socket)
        # Close the connection
        clients.append(client_socket)
        # client_socket.close()
        
    # except BlockingIOError:
    #     pass
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'irc.libera.chat'  # Change this to your server's IP address
    port = 6667  # Change this to the port your server is listening on
    for i in range(1):
        connect_to_server(host, port, i)
    # a = input()
    for i in range(1):
        clients[i].close()

# import socket
# import time


# # Create a new socket using the given pair of family and type
# client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# # Define the server details
# server_ip = "localhost"
# server_port = 6667

# # Connect to the server
# client_socket.connect((server_ip, server_port))

# # Send a message to the server
# message = "pass 2\n"
# client_socket.send(message.encode())

# # Close the connection
# time.sleep(0.01)
# client_socket.close()

