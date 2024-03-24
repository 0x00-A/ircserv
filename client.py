#!/usr/local/bin/python3
from socket import *
from sys import *
import selectors

def    conn(host, port, i):
    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Connect to the server
        client_socket.connect((host, port))
        
        print(f"Connected to {host} on port {port}")
        
        # Send data to the server
        message = f"pass 1\nuser x{i} x x x\nnick user{i}\njoin #ch{i}\n"
        client_socket.sendall(message.encode())
        print("Message sent to server:", message)
        
        # Receive data from the server
        response = client_socket.recv(1024)
        print("Received:", response.decode())
        
        # Close the connection
        clients.append(client_socket)
        # client_socket.close()
        
    except ConnectionRefusedError:
        print("Connection refused. Make sure the server is running and the port is correct.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    host = 'localhost'  # Change this to your server's IP address
    port = 8888  # Change this to the port your server is listening on
    for i in range(10):
        connect_to_server(host, port, i)
    a = input()
    for i in range(10):
        clients[i].close()
        fd = socket(AF_INET, SOCK_STREAM)
        fd.connect((host, port))
        fd.setblocking(False)
        fd.send("pass 2001\r\n".encode())
        fd.send(("nick kk"+str(i)+"\r\n").encode())
        fd.send("user a as s d a \r\n".encode())
        fd.send("join #food\r\n".encode())
        # fd.send(("\n"*1000).encode())
        # fd.send(bytes.fromhex("0a03"))
        return fd
    except Exception as err:
        print("errr :" , err)

if __name__ == "__main__":
    if len(argv) != 4:
        exit(1)
    l = selectors.DefaultSelector()
    for item in range(int(argv[3])):
        s = conn(argv[1], int(argv[2]), item)
        if s:
            l.register(s, selectors.EVENT_READ | selectors.EVENT_WRITE)
    kkkk = True
    while kkkk:
        for k, m in l.select(1):
            con = k.fileobj
            if m & selectors.EVENT_READ:
                data = con.recv(2048)
                print(data.decode(), end='')
                close(k.fd)
