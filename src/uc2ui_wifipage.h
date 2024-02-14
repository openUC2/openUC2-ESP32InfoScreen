#pragma once
#include "lvgl.h"

namespace uc2ui_wifipage
{
    void init_ui(lv_obj_t *wifiPage, void func(lv_event_t * ob));
    void addNetworkToPanel(const char *network);
    void clearNetworks();
    void setOnScanButtonClickListner(void scanbuttonListner());
    void setOnWifiConnectButtonClickListner(void wifiConnectButtonListner(char*,const char*));
    void updatedWifiLed(int lv_pallet_color);
};