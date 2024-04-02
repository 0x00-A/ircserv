import socket
import select

clients = []

def read_from_socket(client_socket):
    response = b""
    while True:
        try:
            chunk = client_socket.recv(4096)
            if not chunk:
                break  # No more data, exit the loop
            response += chunk
        except BlockingIOError:
            break
    print(response.decode())

def connect_to_server(host, port, i):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.setblocking(False)  # Set non-blocking mode

        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        read_from_socket(client_socket)
        
        message = f"user x x x x\nnick lalalala{i}\n"
        client_socket.sendall(message.encode())
        read_from_socket(client_socket)

        for i in range(2):
            message = f"join #chchch{i}\n"
            client_socket.sendall(message.encode())
            print("Message sent to server:", message)
            read_from_socket(client_socket)

        message = f"join #notachannel\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
        read_from_socket(client_socket)
        
        clients.append(client_socket)
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except BlockingIOError:
        pass
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'irc.libera.chat'  # Change this to your server's IP address
    port = 6667
    for i in range(1):
        connect_to_server(host, port, i)
    
    # Wait for data from sockets (non-blocking)
    while clients:
        readable, _, _ = select.select(clients, [], [])
        for sock in readable:
            read_from_socket(sock)
            clients.remove(sock)
            sock.close()
