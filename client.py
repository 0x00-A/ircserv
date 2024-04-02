import socket

clients = []

def read_from_socket(client_socket):
    # Initialize an empty response
    response = b""

    # Keep reading until no more data exists
    while True:
        try:
            chunk = client_socket.recv(4096)
            if not chunk:
                break  # No more data, exit the loop
            response += chunk
        except BlockingIOError:
            # recv would block, break the loop
            break

    # Print the received data
    print(response.decode())

def connect_to_server(host, port, i):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        response = client_socket.recv(10000)
        print("Received:", response.decode())
        
        # Send data to the server
        message = f"user x x x x\nnick lalalala{i}\n"
        client_socket.sendall(message.encode())

        response = client_socket.recv(10000)
        print("Received:", response.decode())

        for i in range(4):
            message = f"join #chchch{i}\n"
            client_socket.sendall(message.encode())
            print("Message sent to server:", message)
        
            # Receive data from the server
            response = client_socket.recv(10000)
            print("Received:", response.decode())

        message = f"join #notachannel\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
    
        # Receive data from the server
        response = client_socket.recv(10000)
        print("Received:", response.decode())
        
        # Close the connection
        clients.append(client_socket)
        # client_socket.close()
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'irc.libera.chat'  # Change this to your server's IP address
    port = 6667  # Change this to the port your server is listening on
    for i in range(1):
        connect_to_server(host, port, i)
    a = input()
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
