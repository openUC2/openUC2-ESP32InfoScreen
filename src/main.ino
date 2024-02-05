
// #define ESP_PANEL_BOARD_ESP32_S3_LCD_EV_BOARD
#define LOOP_DELAY_MS 50 // Delay in milliseconds for the loop
#define ESP_PANEL_CONF_PATH "src/ESP_Panel_Conf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Arduino.h>
#include "esp_flash.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <lvgl.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ArduinoWebsockets.h>
#include "lvgl_controller.h"
//#include "sdsbase64.h" // https://github.com/bluesign/tinyWallet/blob/63351b327f7254f8a367113e28360a97938cdc0c/main/util/sds.h

//#include "WebSocketClientESP32.h"
#include "HTTPClientESP32.h"

// Replace with your actual WiFi credentials
const String ssid = "UC2_LineFrameTrigger";
const String password = "12345678";
const String baseURL = "http://192.168.4.1/";
const String websocket_server_host = "192.168.4.1";

uint16_t websocket_server_port = 80;
using namespace websockets;
WebsocketsClient client;
bool socketConnected = false;
HTTPClientESP32 httpClient(baseURL);



void onEventsCallback(WebsocketsEvent event, String data)
{
  if (event == WebsocketsEvent::ConnectionOpened)
  {
    Serial.println("Connection Opened");
    socketConnected = true;
  }
  else if (event == WebsocketsEvent::ConnectionClosed)
  {
    Serial.println("Connection Closed");
    socketConnected = false;
    ESP.restart();
  }
}

void onMessageCallback(WebsocketsMessage message)
{
  String data = message.data();
  int index = data.indexOf("=");
  if (index != -1)
  {
    String key = data.substring(0, index);
    String value = data.substring(index + 1);

    if (key == "MOVE_FOCUS")
    {
      Serial.println("Moving focus");
    }

    Serial.print("Key: ");
    Serial.println(key);
    Serial.print("Value: ");
    Serial.println(value);
  }
}




void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}

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

  //lv_log_register_print_cb( my_print ); /* register print function for debugging */

  lvgl_controller::initlgvl();

  // display image
//  Serial.println("Displaying image");
//  displayImage(mImage);

  // connecting to Wifi
  /*int nWifiConnectionTrials = 0;
  Serial.println("Connecting to Wifi");
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    nWifiConnectionTrials++;
    if (nWifiConnectionTrials > 10)
      ESP.restart();
  }

  


  // connecting to websocket server
  Serial.println("Connected to WiFi");

  // register callbacks and start the websocket client for frame transfer
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  int nConnectionTrial = 0;
  while (!client.connect(websocket_server_host, websocket_server_port, "/ws"))
  {
    nConnectionTrial++;
    delay(500);
    if (nConnectionTrial > 10)
      ESP.restart();
  }*/


}

// Event handler for button clicks
/*static void button_event_cb(lv_event_t *e)
{
  lv_obj_t *obj = lv_event_get_target(e);

  // Check which button was pressed and perform actions
  if (obj == ui_Button1)
  {
    int stepperid = 1;
    int position = 1000;
    int speed = 1000;
    int isabs = 1;
    int isaccel = 1;
    // send arduino json object to  to server
    // {"motor": { "steppers": [ { "stepperid": 1, "position": 10000, "speed": 15000, "isabs": 0, "isaccel":0} ] } }
    const char *mJson = "{\"motor\": { \"steppers\": [ { \"stepperid\": 1, \"position\": 10000, \"speed\": 15000, \"isabs\": 0, \"isaccel\":0} ] } }";
    client.send(mJson);
    //httpClient.motor_act(stepperid, position, speed, isabs, isaccel);
  }
  else if (obj == ui_Button2)
  {
    int stepperid = 1;
    int position = -1000;
    int speed = 1000;
    int isabs = 1;
    int isaccel = 1;
    const char *mJson = "{\"motor\": { \"steppers\": [ { \"stepperid\": 1, \"position\": -10000, \"speed\": 15000, \"isabs\": 0, \"isaccel\":0} ] } }";
    client.send(mJson);    
    //httpClient.motor_act(stepperid, position, speed, isabs, isaccel);
  }
}*/

void loop()
{
  // Handle LVGL tasks
  //lv_task_handler();
  delay(LOOP_DELAY_MS);
  
  //client.poll();

  // reconnect wifi if connection is lost
  /*if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wifi connection lost");
    ESP.restart();
  }*/
}

/*
lv_img_dsc_t* image_from_base64(char* base64encoded, size_t width, size_t height){
    sds code = sds_base64_decode(base64encoded, strlen(base64encoded));
    size_t data_size = sdslen(code);
    void *data = malloc(data_size);
    memcpy(data, code, data_size);
    sdsfree(code);

    lv_img_dsc_t *dscImage = malloc(sizeof(lv_img_dsc_t));
    dscImage->header.always_zero = 0;
    dscImage->header.w = width;
    dscImage->header.h = height;
    dscImage->header.cf = LV_IMG_CF_RAW_ALPHA;
    dscImage->data = (uint8_t *) data;
    dscImage->data_size = data_size;
    return dscImage;

}
*/    
// decode base64 encoded png image to ps_ram
// https://github.com/OXRS-IO/OXRS-IO-TouchPanel-ESP32-FW
/*lv_img_dsc_t *decodeBase64ToImg(const char *imageBase64)
{
  // exit if no data to decode
  if (strlen(imageBase64) == 0)
    return NULL;

  // decode length 
  size_t outLen = base64::decodeLength(imageBase64);
  uint8_t *raw = (uint8_t *)ps_malloc(outLen);
  base64::decode(imageBase64, raw);

  // prepaare image descriptor
  lv_img_dsc_t *imgPng = (lv_img_dsc_t *)ps_malloc(sizeof(lv_img_dsc_t));
  imgPng->header.cf = LV_IMG_CF_RAW_ALPHA;
  imgPng->header.always_zero = 0;
  imgPng->header.reserved = 0;
  imgPng->header.w = 177;
  imgPng->header.h = 167;
  imgPng->data_size = outLen;
  imgPng->data = raw;

  return imgPng;
}*/



/*void displayImage(const char *base64Image) {
    lv_img_dsc_t *imgDsc = decodeBase64ToImg(base64Image);  
    if (imgDsc == NULL){
      Serial.println("Image could not be decoded");
      return;
    }

    Serial.println("Image decoded");
    lv_obj_t *img = lv_img_create(ui_TabPage3);
    lv_img_set_src(img, imgDsc);*/
        
    /*
    lv_obj_t * icon = lv_image_create(lv_screen_active(), NULL);
    lv_image_set_src(icon, &my_icon_dsc);
    */
    //lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    // You might need to handle memory management (freeing old images etc.)
//}


/*
#include "lvgl.h"
#include "base64.h"
#include "TJpgDec.h"  // JPEG decoding library
lv_img_dsc_t *decodeBase64ToJpegImg(const char *imageBase64) {
    if (strlen(imageBase64) == 0) return NULL;

    size_t outLen = base64::decodeLength(imageBase64);
    uint8_t *jpegData = (uint8_t *)ps_malloc(outLen);
    base64::decode(imageBase64, jpegData);

    // Use TJpgDec or another JPEG decoding library to decode jpegData
    // into raw pixel data. You'll need to get the width, height, and
    // format of the image from the JPEG header.

    // Example placeholder code
    uint8_t *rawPixelData;
    uint32_t width, height;
    TJpgDec::getInfo(jpegData, outLen, &width, &height);
    rawPixelData = (uint8_t *)ps_malloc(width * height * 3); // For RGB888 format
    TJpgDec::decompress(rawPixelData, width, jpegData, outLen);

    free(jpegData); // Free the JPEG data after decompression

    // Prepare the image descriptor
    lv_img_dsc_t *imgDsc = (lv_img_dsc_t *)ps_malloc(sizeof(lv_img_dsc_t));
    imgDsc->header.cf = LV_IMG_CF_TRUE_COLOR; // Assuming RGB888 format
    imgDsc->header.w = width;
    imgDsc->header.h = height;
    imgDsc->data_size = width * height * 3;
    imgDsc->data = rawPixelData;

    return imgDsc;
}
*/