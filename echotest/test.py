import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ("138.68.40.50", 10000)
print('connecting to %s port %s' % server_address)
sock.connect(server_address)

