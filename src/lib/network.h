#pragma once

#include "rpc.h"

class Network {

public:
    void connect(std::string ssid, std::string password);
    bool is_connected() { return connected; }
    std::string ip_address() { return ip_addr; }
    std::string mac_address();
    std::string status() { return status_msg; }
    int epoch();

    void mdns(std::string hostname, std::string friendly_name);
    void mdns_service(std::string type, std::string proto, int port);
    void ota(std::string url, std::string cert_pem, bool skip_cert_check);
    void ota_invalid();   // mark running image invalid and reboot

private:
    bool sta_initialized = false;
    bool sntp_started = false;
    bool connected = false;
    std::string ip_addr;
    std::string status_msg = "idle";
};
