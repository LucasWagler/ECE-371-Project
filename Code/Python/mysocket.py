import socket
import sys

soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
soc.connect(('192.168.0.222', 80))

msg = soc.recv(1024)
print(msg.decode("utf-8"))