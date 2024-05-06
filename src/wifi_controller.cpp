#include "wifi_controller.h"
#include <WiFi.h>
#include "uc2ui_wifipage.h"
#include "mdns.h"
#include "RestApi.h"

namespace wifi_controller
{

    wifi_status current_state = wifi_status::disconnected;

    void scanForNetworks()
    {
        int networkcount = WiFi.scanNetworks();
        uc2ui_wifipage::clearNetworks();
        for (int i = 0; i < networkcount; i++)
        {
            uc2ui_wifipage::addNetworkToPanel(WiFi.SSID(i).c_str());
        }
    }

    void connectToNetwork(char *ssid, const char *pw)
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
        if (ret == 5)
            WiFi.disconnect();
    }

    void loop()
    {
        wifi_status newstate = WiFi.status() == WL_CONNECTED ? wifi_status::connected : wifi_status::disconnected;
        if (newstate != current_state)
        {
            current_state = newstate;
            if (current_state == wifi_status::connected)
                uc2ui_wifipage::updatedWifiLed(9); // green
            if (current_state == wifi_status::disconnected)
                uc2ui_wifipage::updatedWifiLed(0); // red
        }
    }

    char *searchForDevices()
    {
        mdns_init();
        mdns_result_t *results = NULL;
        esp_err_t err = mdns_query_ptr("_http", "_tcp", 3000, 20, &results);
        if (err)
        {
            log_e("Query Failed");
            return NULL;
        }
        if (!results)
        {
            log_w("No results found!");
            return NULL;
        }
        char *device = "";
        while (results)
        {
            if (results->hostname)
            {
                device = results->hostname;
                uc2ui_wifipage::addDeviceToPanel(results->hostname);
                results = results->next;
            }
        }
        mdns_query_results_free(results);
        mdns_free();
        return device;
    }

    void connectToDevice(char *url)
    {
        mdns_init();
        esp_ip4_addr_t address;

        esp_err_t err = mdns_query_a(url, 2000, (esp_ip4_addr_t *)&address);
        if (err)
        {
            if (err == ESP_ERR_NOT_FOUND)
            {
                printf("Host was not found!");
                return;
            }
            printf("Query Failed");
            return;
        }
        char result[16];

        sprintf(result, "%d.%d.%d.%d",
                (address.addr) & 0xFF,
                (address.addr >> 8) & 0xFF,
                (address.addr >> 16) & 0xFF,
                (address.addr >> 24) & 0xFF);
        String s = "http://";
        s += result;
        log_i("connect to %s with ip %s %i", url, s.c_str(), address.addr);
        RestApi::connectTo(s);

        // printf(IPSTR, IP2STR(&address));
        mdns_free();
    }

    void autoConnect(){
        // try to connect to the last known network
        // 1. Search for SSID:UC2_3 @ PW:12345678
        // 2. list devices and connect to devive 0 
        // 3. connect to host
        const char* ssid     = "Uc2";
        const char* password = "12345678";
        log_i("Connect to: %s PW:%s", ssid, password);
        // scan for networks
        int networkcount = WiFi.scanNetworks();
        for (int i = 0; i < networkcount; i++)
        {
            log_i("Found network %s", WiFi.SSID(i));
        }

        WiFi.begin(ssid, password);
        log_i("Connecting to WiFi...");

        int ret = 0;
        while (WiFi.status() != WL_CONNECTED && ret < 5)
        {
            delay(1000);
            ret++;
        }
        WiFi.setAutoReconnect(false);
        if (ret == 5){
            WiFi.disconnect();
            }
        else{
            log_i("search for devices");
            char *mUrl = searchForDevices();
            if (mUrl != NULL)
            {
                log_i("connect to device: %s", mUrl);
                connectToDevice(mUrl);
            }
        }
    }
}