#pragma once

#include "Arduino.h"

class Network {

public:
    void connect(String &ssid, String &password);
    bool is_connected() { return connected; }
    String ip_address() { return ip_addr; }
    String status() { return status_msg; }
    int epoch();

    void mdns(String &hostname, String &friendly_name);
    void mdns_service(String &type, String &proto, int port);
    void ota(String &url, String &cert_pem, bool skip_cert_check);
    void ota_invalid();   // mark running image invalid and reboot

    bool sta_initialized = false;
    bool sntp_started = false;
    bool connected = false;
    String ip_addr;
    String status_msg = "idle";
};
