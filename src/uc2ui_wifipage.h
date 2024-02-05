#pragma once
#include "lvgl.h"

namespace uc2ui_wifipage
{
    void init_ui(lv_obj_t *wifiPage, void func(lv_event_t * ob));
    void addNetworksToPanel(char * networks[], int len);
    void setOnScanButtonClickListner(void scanbuttonListner());
    void setOnWifiConnectButtonClickListner(void wifiConnectButtonListner());
};