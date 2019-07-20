#include "sock.h"
#include "vector.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "socket";

// note: lwip_..._r (_r) is "thread safe"
// https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/lwip/lwip/sockets.h

Object<int, int, int, String, Object<String, String>> socket_getaddrinfo(String &host, String &port, int af, int type, int proto, int flags)
{
    struct addrinfo hints, *res;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = af;
    hints.ai_socktype = type;
    hints.ai_protocol = proto;
    hints.ai_flags = flags;

    auto status = lwip_getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (status != 0) throw "host not found";

    Object<int, int, int, String, Object<String, String>> result;

    struct addrinfo *p = res;
    if (p == NULL) throw "no addresses found";

    // return only the 1st result
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
    in_addr *addr_ = &(ipv4->sin_addr);

    // convert ip address to string
    inet_ntop(p->ai_family, addr_, ipstr, sizeof ipstr);

    get<0>(result) = p->ai_family;
    get<1>(result) = p->ai_socktype;
    get<2>(result) = p->ai_protocol;
    get<3>(result) = String(p->ai_canonname);
    get<0>(get<4>(result)) = ipstr;
    get<1>(get<4>(result)) = port.c_str();

    freeaddrinfo(res);
    return result;
}

int socket_socket(int domain, int type, int protocol)
{
    auto fd = lwip_socket(domain, type, protocol);
    if (fd < 0) throw strerror(errno);
    return fd;
}

int socket_bind(int fd, String &host, String &port)
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
        ESP_LOGI(TAG, "bind.setsocketopt REUSEADDR returns %d %s", errno, strerror(errno));
    
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    const char *host_ = host.c_str();
    if (host.length() == 0) {
        hints.ai_flags = AI_PASSIVE;
        host_ = NULL;
    }
    int r = getaddrinfo(host_, port.c_str(), &hints, &res);
    if (r != 0) throw strerror(r);
    r = lwip_bind_r(fd, res->ai_addr, res->ai_addrlen);
    lwip_freeaddrinfo(res);
    if (r < 0) throw strerror(errno);
    return 0;
}

int socket_setsockopt(int fd, int level, int optname, String &value)
{
    int r = lwip_setsockopt_r(fd, level, optname, value.c_str(), value.length());
    if (r != 0) throw strerror(errno);
    return r;
}

int sock_listen(int fd, int backlog)
{
    int r = lwip_listen_r(fd, backlog);
    if (r < 0) throw strerror(errno);
    return r;
}

Object<int, Object<String, int>> socket_accept(int fd)
{
    struct sockaddr remote_addr;
    auto addr_size = sizeof remote_addr;
    int new_fd = lwip_accept_r(fd, &remote_addr, (socklen_t*)&addr_size);
    if (new_fd < 0) throw strerror(errno);
    Object<int, Object<String, int>> client;
    uint8_t *ip = (uint8_t*)&((struct sockaddr_in *)&remote_addr)->sin_addr;
    char ip_str[16];
    snprintf(ip_str, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    int port = lwip_ntohs(((struct sockaddr_in *)&remote_addr)->sin_port);
    get<0>(client) = new_fd;
    get<0>(get<1>(client)) = String(ip_str);
    get<1>(get<1>(client)) = port;
    return client;
}

int socket_connect(int fd, String &host, String &port)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int r = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (r != 0) throw strerror(r);
    r = lwip_connect_r(fd, res->ai_addr, res->ai_addrlen);
    if (res) lwip_freeaddrinfo(res);
    if (r != 0) throw strerror(errno);
    return 0;
}

int sock_send(int fd, Vector<uint8_t> &data)
{
    uint8_t *d = data.data();
    int l = lwip_send_r(fd, d, data.size, 0);
    if (l < 0) throw strerror(errno);
    return l;
}

int sock_sendall(int fd, int timeout_ms, Vector<uint8_t> &data)
{
    int sentlen = 0;
    TickType_t stop = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);
    while (sentlen < data.size && xTaskGetTickCount() < stop)
    {
        int r = lwip_send_r(fd, data.data()+sentlen, data.size-sentlen, 0);
        ESP_LOGI(TAG, "sendall sent %d so far %d, total %d", r, sentlen, data.size);
        if (r < 0 && errno != EWOULDBLOCK) throw strerror(errno);
        if (r > 0) sentlen += r;
    }
    if (sentlen < data.size) throw "TimeoutError";
    return sentlen;
}

int sock_sendto(int fd, String &host, String &port, Vector<uint8_t> &data)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int r = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (r != 0) throw strerror(r);
    r = lwip_sendto_r(fd, data.data(), data.size, 0, res->ai_addr, res->ai_addrlen);
    lwip_freeaddrinfo(res);
    if (r < 0) throw strerror(errno);
    return r;
}

Object<Vector<uint8_t>, Object<String, int>> sock_recvfrom(int fd, int len)
{
    struct sockaddr remote_addr;
    auto addr_size = sizeof remote_addr;
    uint8_t buffer[len];
    int l = lwip_recvfrom_r(fd, buffer, len, 0, &remote_addr, (socklen_t *)&addr_size);
    if (l < 0) throw strerror(errno);
    Object<Vector<uint8_t>, Object<String, int>> result;
    const char *ip = (const char *)&((struct sockaddr_in *)&remote_addr)->sin_addr;
    int port = lwip_ntohs(((struct sockaddr_in *)&remote_addr)->sin_port);
    get<0>(result) = Vector<uint8_t>(l);
    for (int i=0;  i<l;  i++)
        get<0>(result)[i] = buffer[i];
    get<0>(get<1>(result)) = String(ip);
    get<1>(get<1>(result)) = port;
    return result;
}

Vector<uint8_t> sock_recv(int fd, int len)
{
    uint8_t buffer[len];
    int l = lwip_recv_r(fd, buffer, len, 0);
    if (l < 0) throw strerror(errno);
    ESP_LOGI(TAG, "recv got %d of %d bytes", l, len);
    Vector<uint8_t> res(l);
    for (int i=0;  i<l;  i++) res[i] = buffer[i];
    return res;
}

int socket_close(int fd)
{
    return lwip_close_r(fd);
}
