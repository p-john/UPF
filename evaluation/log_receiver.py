__author__ = 'Asto_2'
import os
from cb_generate import generate_cb_file, generate_trace_file
import socket
import os, argparse

parser = argparse.ArgumentParser(description='Logger')
parser.add_argument('-o', '--output', help='Output file', required=True)
args = parser.parse_args()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((" ", 5555))

with open(os.path.join(args.output), 'a+') as f:
    while True:
        data = sock.recv(1024)  # buffer size is 1024 bytes
        print(data.decode("utf-8"))
        f.write(data.decode("utf-8") + "\n")
        if data.decode("utf-8") == "FINAL PACKET":
            break
    sock.close()