#include <Arduino.h>
#include "nvs_flash.h"
#include "lvgl_controller.h"
#include "wifi_controller.h"
#include "uc2ui_wifipage.h"
#include "uc2ui_controlpage.h"
#include "RestApi.h"

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */
  Serial.println("");

  delay(1000);

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    Serial.println("nvs init err");
  }

  lvgl_controller::initlgvl();

  uc2ui_wifipage::setOnScanButtonClickListner(wifi_controller::scanForNetworks);
  uc2ui_wifipage::setOnWifiConnectButtonClickListner(wifi_controller::connectToNetwork);

  uc2ui_controlpage::setColorChangedListner(RestApi::websocket_updateColors);
  uc2ui_controlpage::setConnectToHostListner(RestApi::connectTo);
}

void loop()
{
  delay(5);
}
