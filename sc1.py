import socket

def client():
    # Define server address and port
    server_address = ('irc.libera.chat', 6667)

    # Create a socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to the server
        client_socket.connect(server_address)
        print("Connected to the server.")

        while True:
            # Get user input
            message = input("Enter a message to send to the server (or 'exit' to quit): ")

            # Send the message to the server
            client_socket.sendall(message.encode())

            # Receive response from the server
            response = client_socket.recv(1024).decode()
            print(f"Server response: {response}")

            # Check if user wants to exit
            if message.lower() == 'exit':
                break

    except ConnectionRefusedError:
        print("Error: Could not connect to the server.")
    finally:
        # Close the socket
        client_socket.close()

# Run the client
client()
