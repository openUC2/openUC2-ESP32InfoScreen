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
    lv_obj_t *statusLabel;
    lv_obj_t *statusLED;
    lv_obj_t *devicesPanel;
    lv_obj_t *devicesLabel;
    lv_obj_t *devicesButton;
    lv_obj_t *devicesButtonLabel;
    // focus event for textareas to trigger the keyboard visibility
    void (*fev)(lv_event_t *ob);
    void (*_scanButtonListner)();
    void (*_searchDeviceButtonListner)();
    void (*_wifiConnectButtonListner)(char *, const char *);
    char *networkid;
    char*deviceid;

    void setOnSearchDeviceButtonClickListner(void searchDeviceListner())
    {
        _searchDeviceButtonListner = searchDeviceListner;
    }

    void setOnScanButtonClickListner(void scanbuttonListner())
    {
        _scanButtonListner = scanbuttonListner;
    }
    void setOnWifiConnectButtonClickListner(void wifiConnectButtonListner(char *, const char *))
    {
        _wifiConnectButtonListner = wifiConnectButtonListner;
    }

    void (*connectToHost)(char* s);

    void setConnectToHostListner(void contoHst(char* s))
    {
        connectToHost = contoHst;
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
            const char *pw = lv_textarea_get_text(wifiPwTextArea);
            _wifiConnectButtonListner(networkid, pw);
        }
        else if(obj == devicesButton && _searchDeviceButtonListner != nullptr)
        {
            _searchDeviceButtonListner();
        }
    }

    void network_item_clicked_cb(lv_event_t *e)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        lv_obj_t *lbl = lv_obj_get_child(obj, NULL);
        networkid = lv_label_get_text(lbl);
        log_i("NetworkSSID clicked:%s", networkid);
    }
    void device_item_clicked_cb(lv_event_t *e)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        lv_obj_t *lbl = lv_obj_get_child(obj, NULL);
        deviceid = lv_label_get_text(lbl);
        connectToHost(deviceid);
        log_i("DeviceId clicked:%s", deviceid);
    }

    void updatedWifiLed(int lv_pallet_color)
    {
        log_i("update led color:%i", lv_pallet_color);
        lv_led_set_color(statusLED, lv_palette_main(static_cast<lv_palette_t>(lv_pallet_color)));
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
        lv_obj_set_flex_flow(networksPanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(networksPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

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

        statusLabel = lv_label_create(wifiPage);
        lv_obj_set_width(statusLabel, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(statusLabel, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(statusLabel, -111);
        lv_obj_set_y(statusLabel, -171);
        lv_obj_set_align(statusLabel, LV_ALIGN_CENTER);
        lv_label_set_text(statusLabel, "Status:");

        statusLED = lv_led_create(wifiPage);
        lv_obj_set_x(statusLED, -55);
        lv_obj_set_y(statusLED, -171);
        lv_obj_set_align(statusLED, LV_ALIGN_CENTER);
        lv_led_on(statusLED);
        lv_led_set_color(statusLED, lv_palette_main(LV_PALETTE_RED));

        devicesPanel = lv_obj_create(wifiPage);
        lv_obj_set_width(devicesPanel, 248);
        lv_obj_set_height(devicesPanel, 221);
        lv_obj_set_x(devicesPanel, 511);
        lv_obj_set_y(devicesPanel, 87);
        lv_obj_clear_flag(devicesPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags

        devicesLabel = lv_label_create(wifiPage);
        lv_obj_set_width(devicesLabel, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(devicesLabel, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(devicesLabel, 164);
        lv_obj_set_y(devicesLabel, -124);
        lv_obj_set_align(devicesLabel, LV_ALIGN_CENTER);
        lv_label_set_text(devicesLabel, "Devices:");

        devicesButton = lv_btn_create(wifiPage);
        lv_obj_set_width(devicesButton, 132);
        lv_obj_set_height(devicesButton, 50);
        lv_obj_set_x(devicesButton, 203);
        lv_obj_set_y(devicesButton, -168);
        lv_obj_set_align(devicesButton, LV_ALIGN_CENTER);
        lv_obj_add_flag(devicesButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
        lv_obj_clear_flag(devicesButton, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
        lv_obj_add_event_cb(devicesButton, button_event_cb, LV_EVENT_CLICKED, NULL);

        devicesButtonLabel = lv_label_create(devicesButton);
        lv_obj_set_width(devicesButtonLabel, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(devicesButtonLabel, LV_SIZE_CONTENT); /// 1
        lv_obj_set_align(devicesButtonLabel, LV_ALIGN_CENTER);
        lv_label_set_text(devicesButtonLabel, "Search Devices");
    }

    void clearNetworks()
    {
        lv_obj_clean(networksPanel);
    }

    void addButtonToPanel(const char *name, lv_obj_t * panel, void(item_clicked)(lv_event_t * cb))
    {
        lv_obj_t *button = lv_btn_create(panel);
        lv_obj_set_width(button, LV_SIZE_CONTENT);
        lv_obj_set_height(button, 30);
        lv_obj_add_flag(button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(button);
        lv_obj_set_width(label, LV_SIZE_CONTENT);
        lv_obj_set_height(label, LV_SIZE_CONTENT);
        lv_obj_set_align(label, LV_ALIGN_CENTER);
        lv_label_set_text(label, name);
        lv_obj_add_event_cb(button, item_clicked, LV_EVENT_CLICKED, NULL);
    }

    void addNetworkToPanel(const char *network)
    {
        addButtonToPanel(network,networksPanel, network_item_clicked_cb);
    }

    void addDeviceToPanel(const char * device)
    {
        addButtonToPanel(device,devicesPanel,device_item_clicked_cb);
    }

    

}