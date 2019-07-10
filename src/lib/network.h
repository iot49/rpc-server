#pragma once

#include "Arduino.h"

class Network {

public:
    void connect(String ssid, String password);
    bool is_connected() { return connected; }
    String ip_address() { return ip_addr; }
    String status() { return status_msg; }
    int epoch();

    bool sta_initialized = false;
    bool sntp_started = false;
    bool connected = false;
    String ip_addr;
    String status_msg = "idle";
};
