#pragma once

#include "Arduino.h"
#include "vector.h"
#include "tuple.h"

Object<int, int, int, String, Object<String, String>> socket_getaddrinfo(String &host, String &port, int af, int type, int proto, int flags);
int socket_socket(int domain, int type, int protocol);
int socket_setsockopt(int fd, int level, int optname, String &value);
int socket_bind(int fd, String &host, String &port);
int sock_listen(int fd, int backlog);
Object<int, Object<String, int>> socket_accept(int fd);
int socket_connect(int fd, String &host, String &port);
int sock_send(int fd, Vector<uint8_t> &data);
int sock_sendall(int fd, int timeout_ms, Vector<uint8_t> &data);
int sock_sendto(int fd, String &host, String &port, Vector<uint8_t> &data);
Vector<uint8_t> sock_recv(int fd, int len);
Object<Vector<uint8_t>, Object<String, int>> sock_recvfrom(int fd, int len);
int socket_close(int fd);
