import socket

def connect_to_server(host, port):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = "PASS   1235\nNICK 9SDF\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
        
        # Receive data from the server
        # response = client_socket.recv(1024)
        # print("Received:", response.decode())
        
        # Close the connection
        client_socket.close()
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'localhost'  # Change this to your server's IP address
    port = 8080  # Change this to the port your server is listening on
    connect_to_server(host, port)