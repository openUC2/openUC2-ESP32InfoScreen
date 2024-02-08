#pragma once

namespace wifi_controller
{
    void scanForNetworks();
    void connectToNetwork(char * ssid,const char * pw);
}; // namespace wifi_controller
