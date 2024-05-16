#pragma once
#include "Arduino.h"

namespace wifi_controller
{

    enum wifi_status
    {
        disconnected,
        pending,
        connected,
    };

    void scanForNetworks();
    void connectToNetwork(char * ssid,const char * pw);
    void loop();
    char* searchForDevices();
    void connectToDevice(char* url);
    bool autoConnect();
}; // namespace wifi_controller
