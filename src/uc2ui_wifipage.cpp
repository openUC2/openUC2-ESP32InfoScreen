#include "uc2ui_wifipage.h"
#include "Arduino.h"

namespace uc2ui_wifipage
{
    lv_obj_t *networksLabel;
    lv_obj_t *networksPanel;
    static lv_obj_t *buttonScan;
    lv_obj_t *label1;
    static lv_obj_t *wifiPwTextArea;
    static lv_obj_t *buttonConnect;
    lv_obj_t *label2;
    // focus event for textareas to trigger the keyboard visibility
    void (*fev)(lv_event_t *ob);
    void (*_scanButtonListner)();
    void (*_wifiConnectButtonListner)();

    void setOnScanButtonClickListner(void scanbuttonListner())
    {
        _scanButtonListner = scanbuttonListner;
    }
    void setOnWifiConnectButtonClickListner(void wifiConnectButtonListner())
    {
        _wifiConnectButtonListner = wifiConnectButtonListner;
    }

    void button_event_cb(lv_event_t *e)
    {
        lv_obj_t *obj = lv_event_get_target(e);

        // Check which button was pressed and perform actions
        if (obj == buttonScan && _scanButtonListner != nullptr)
        {
            log_i("scanbuttonclick");
            _scanButtonListner();
        }
        else if (obj == buttonConnect && _wifiConnectButtonListner != nullptr)
        {
            log_i("_wifiConnectButtonListner");
            _wifiConnectButtonListner();
        }
    }

    void init_ui(lv_obj_t *wifiPage, void func(lv_event_t *ob))
    {
        fev = func;
        buttonScan = lv_btn_create(wifiPage);
        lv_obj_set_width(buttonScan, 100);
        lv_obj_set_height(buttonScan, 50);
        lv_obj_add_flag(buttonScan, LV_OBJ_FLAG_SCROLL_ON_FOCUS); 
        lv_obj_clear_flag(buttonScan, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(buttonScan, button_event_cb, LV_EVENT_CLICKED, NULL);

        label1 = lv_label_create(buttonScan);
        lv_obj_set_width(label1, LV_SIZE_CONTENT);
        lv_obj_set_height(label1, LV_SIZE_CONTENT);
        lv_obj_set_align(label1, LV_ALIGN_CENTER);
        lv_label_set_text(label1, "Start Scan");

        networksPanel = lv_obj_create(wifiPage);
        lv_obj_set_width(networksPanel, 494);
        lv_obj_set_height(networksPanel, 221);
        lv_obj_set_x(networksPanel, 3);
        lv_obj_set_y(networksPanel, 87);

        networksLabel = lv_label_create(wifiPage);
        lv_obj_set_width(networksLabel, LV_SIZE_CONTENT);
        lv_obj_set_height(networksLabel, LV_SIZE_CONTENT);
        lv_obj_set_x(networksLabel, -339);
        lv_obj_set_y(networksLabel, -126);
        lv_obj_set_align(networksLabel, LV_ALIGN_CENTER);
        lv_label_set_text(networksLabel, "Networks:");

        wifiPwTextArea = lv_textarea_create(wifiPage);
        lv_obj_set_width(wifiPwTextArea, 358);
        lv_obj_set_height(wifiPwTextArea, LV_SIZE_CONTENT); /// 70
        lv_obj_set_x(wifiPwTextArea, -194);
        lv_obj_set_y(wifiPwTextArea, 144);
        lv_obj_set_align(wifiPwTextArea, LV_ALIGN_CENTER);
        lv_textarea_set_placeholder_text(wifiPwTextArea, "WifiPassword...");
        lv_textarea_set_one_line(wifiPwTextArea, true);
        // register de/focus events to show/hide keyboard
        lv_obj_add_event_cb(wifiPwTextArea, fev, LV_EVENT_ALL, NULL);

        buttonConnect = lv_btn_create(wifiPage);
        lv_obj_set_width(buttonConnect, 100);
        lv_obj_set_height(buttonConnect, 50);
        lv_obj_set_x(buttonConnect, 65);
        lv_obj_set_y(buttonConnect, 145);
        lv_obj_set_align(buttonConnect, LV_ALIGN_CENTER);
        lv_obj_add_flag(buttonConnect, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(buttonConnect, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(buttonConnect, button_event_cb, LV_EVENT_CLICKED, NULL);

        label2 = lv_label_create(buttonConnect);
        lv_obj_set_width(label2, LV_SIZE_CONTENT);
        lv_obj_set_height(label2, LV_SIZE_CONTENT);
        lv_obj_set_align(label2, LV_ALIGN_CENTER);
        lv_label_set_text(label2, "Connect");
    }

    void addNetworkToPanel(char *network)
    {
        lv_obj_t *label = lv_label_create(networksPanel);
        lv_obj_set_width(label, LV_SIZE_CONTENT);
        lv_obj_set_height(label, LV_SIZE_CONTENT);
        lv_obj_set_align(label, LV_ALIGN_CENTER);
        lv_label_set_text(label, network);
    }

    void addNetworksToPanel(char *networks[], int len)
    {
        for (int i = 0; i < len; i++)
        {
            addNetworkToPanel(networks[i]);
        }
    }
}