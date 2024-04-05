import socket
import random

clients = []

def connect_to_server(host, port, nickname):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = f"PASS 1\r\n"
        client_socket.sendall(message.encode())
        message = f"NICK {nickname}\r\n"
        client_socket.sendall(message.encode())
        message = f"USER {nickname} 0 * :{nickname}\r\n"
        client_socket.sendall(message.encode())
        message = f"JOIN #food\r\n"
        client_socket.sendall(message.encode())
        
        print("Messages sent to server")
        
        # Add the client socket to the list
        clients.append(client_socket)
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'localhost'  # Change this to your server's IP address
    port = 6667  # Change this to the port your server is listening on
    
    # Connect 100 clients
    for i in range(1000):
        nickname = f"user{i}"
        connect_to_server(host, port, nickname)
    
    # Wait for input to close the connections
    input("Press Enter to close the connections...")
    
    # Close all client connections
    for client_socket in clients:
        client_socket.close()
