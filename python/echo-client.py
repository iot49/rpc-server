#!/usr/bin/env python3

import socket

HOST = '10.95.10.106'  # The server's hostname or IP address
PORT = 8996            # The port used by the server

code_samples = [
"7*7",
"2**234",
"""
sum = 0
for i in range(10):
    sum += i
    print('{}^4 = {}'.format(i, i**4))
print(sum)
""",
"print(5*9)"
]

for code in code_samples:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(code.encode())
        print("-------------- EVAL {}\n===\n".format(code))
        while True:
            result = s.recv(1024)
            if len(result) == 0: 
                print("\nDONE\n\n\n")
                break
            try:
                result = result.decode()
            except:
                pass
            print(result, end="")
            # print("got '{}'".format(result))
