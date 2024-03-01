#include "wifi_controller.h"
#include <WiFi.h>
#include "uc2ui_wifipage.h"

namespace wifi_controller
{

    wifi_status current_state = wifi_status::disconnected;

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
        uc2ui_wifipage::updatedWifiLed(14);
        WiFi.begin(ssid, pw);
        
        int ret = 0;
        while (WiFi.status() != WL_CONNECTED && ret < 5)
        {
            delay(1000);
            ret++;
        }
        WiFi.setAutoReconnect(false);
        if(ret == 5)
            WiFi.disconnect();
    }

    void loop()
    {
        wifi_status newstate = WiFi.status() == WL_CONNECTED ? wifi_status::connected : wifi_status::disconnected;
        if(newstate != current_state)
        {
            current_state = newstate;
            if(current_state == wifi_status::connected)
                uc2ui_wifipage::updatedWifiLed(9);//green
            if(current_state == wifi_status::disconnected)
                uc2ui_wifipage::updatedWifiLed(0);//red
        }

        
    }
}