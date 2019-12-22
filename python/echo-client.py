#!/usr/bin/env python3

import socket

HOST = '10.95.10.106'  # The server's hostname or IP address
PORT = 8996            # The port used by the server

code_samples = [
"7*7",
"2**234",
"""
sum = 0
for i in range(5):
    sum += i*i
    print('power {:5d}^{} = {}'.format(i, i, i**i))
print('sum =', sum)
""",
"""
def foo():
    print('raise ValueError')
    raise ValueError('what value?')
a = 5
print("3*a =", 3*a)
foo()
print('past the error')
""",
]

for code in code_samples:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(code.encode())
        print("\n-------------- EVAL {}\n===".format(code))
        while True:
            result = s.recv(1024)
            if len(result) == 0:
                break
            try:
                result = result.decode()
            except:
                pass
            print(result, end="")
