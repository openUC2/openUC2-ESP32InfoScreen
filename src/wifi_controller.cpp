#include "wifi_controller.h"
#include <WiFi.h>
#include "uc2ui_wifipage.h"

namespace wifi_controller
{
    void scanForNetworks()
    {
        int networkcount = WiFi.scanNetworks();
        uc2ui_wifipage::clearNetworks();
        for(int i =0;i< networkcount;i++)
        {
            uc2ui_wifipage::addNetworkToPanel(WiFi.SSID(i).c_str());
        }
    }

    void connectToNetwork(char * ssid,const char * pw)
    {
        log_i("Connect to: %s PW:%s", ssid, pw);
        WiFi.begin(ssid, pw);
        int ret = 0;
        while (WiFi.status() != WL_CONNECTED && ret < 5)
        {
            delay(1000);
            ret++;
        }
        if(ret == 5)
            WiFi.disconnect();
    }
}