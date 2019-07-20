#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
HOST = '0.0.0.0'
PORT = 8996         # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(6)
    while True:
        try:
            print("accepting connections at {}:{}".format(HOST, PORT))
            conn, addr = s.accept()
            print("conn = {}".format(conn))
            print("addr = {}".format(addr))
            with conn:
                msg = b'just a initial hello'
                print("sendall {} {}={}".format(msg, conn.sendall(msg), len(msg)))
                while True:
                    print("waiting for data ...")
                    data = conn.recv(1024)
                    print("got", data)
                    if not data or len(data) ==0:
                        break
                    conn.sendall(data)
        except:
            pass
