import socket
import time

clients = []

def connect_to_server(host, port, i):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = f"pass 2001\nuser x{i} x x x\nnick galta{i}\njoin #food\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
        
        # Receive data from the server
        # response = client_socket.recv(1024)
        # print("Received:", response.decode())
        
        # Close the connection
        clients.append(client_socket)
        # client_socket.close()
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'e1r2p15'  # Change this to your server's IP address
    port = 8080  # Change this to the port your server is listening on
    for i in range(30):
        connect_to_server(host, port, i)
    while True:
        for i in range(len(clients)):  # Use len(clients) to handle dynamic list
            try:
                client_socket = clients[i]
                client_socket.setblocking(False)  # Make the socket non-blocking

                response = client_socket.recv(1024)
                if response:  # Data received
                    print("Received:", i, response.decode())

            except BlockingIOError:
                # No data yet, move on to the next client
                pass 
            except Exception as e:
                print(f"Error with client {i}:", e)
                clients[i].close()  # Close on errors

            time.sleep(0.1)  # Small delay to avoid overloading the server
    for i in range(30):
        clients[i].close()