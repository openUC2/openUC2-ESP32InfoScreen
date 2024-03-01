#pragma once

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
}; // namespace wifi_controller
