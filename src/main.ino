
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
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ui.h>
#include <ArduinoWebsockets.h>

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


/* LVGL porting configurations */
#define LVGL_TICK_PERIOD_MS (2)
#define LVGL_TASK_MAX_DELAY_MS (500)
#define LVGL_TASK_MIN_DELAY_MS (1)
#define LVGL_TASK_STACK_SIZE (4 * 1024)
#define LVGL_TASK_PRIORITY (2)
#define LVGL_BUF_SIZE (ESP_PANEL_LCD_H_RES * 20)

ESP_Panel *panel = NULL;
SemaphoreHandle_t lvgl_mux = NULL; // LVGL mutex

/**
 * LVGL Porting Functions
 *
 * These code provide functions for handling display flushing, touch input, and LVGL task
 *
 */
/* Display flushing */
void lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
  lv_disp_flush_ready(disp);
}
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

/* Read the touchpad */
#ifdef ESP_PANEL_USE_LCD_TOUCH
void lvgl_port_tp_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
  panel->getLcdTouch()->readData();

  bool touched = panel->getLcdTouch()->getTouchState();
  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    TouchPoint point = panel->getLcdTouch()->getPoint();

    data->state = LV_INDEV_STATE_PR;
    /*Set the coordinates*/
    data->point.x = point.x;
    data->point.y = point.y;

    // Serial.printf("Touch point: x %d, y %d\n", point.x, point.y);
  }
}
#endif

void lvgl_port_lock(int timeout_ms)
{
  const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
  xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks);
}

void lvgl_port_unlock(void)
{
  xSemaphoreGiveRecursive(lvgl_mux);
}

void lvgl_port_task(void *arg)
{
  Serial.println("Starting LVGL task");

  uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
  while (1)
  {
    // Lock the mutex due to the LVGL APIs are not thread-safe
    lvgl_port_lock(-1);
    task_delay_ms = lv_timer_handler();
    // Release the mutex
    lvgl_port_unlock();
    if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS)
    {
      task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    }
    else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS)
    {
      task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
    }
    vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
  }
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

  panel = new ESP_Panel();

  /* Initialize LVGL core */
  lv_init();

  /* Initialize LVGL buffers */
  static lv_disp_draw_buf_t draw_buf;
  /* Using double buffers is more faster than single buffer */
  /* Using internal SRAM is more fast than PSRAM (Note: Memory allocated using `malloc` may be located in PSRAM.) */
  uint8_t *buf = (uint8_t *)heap_caps_calloc(1, LVGL_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL);
  assert(buf);
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_BUF_SIZE);

  /* Initialize the display device */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = ESP_PANEL_LCD_H_RES;
  disp_drv.ver_res = ESP_PANEL_LCD_V_RES;
  disp_drv.flush_cb = lvgl_port_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the input device */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = lvgl_port_tp_read;
  lv_indev_drv_register(&indev_drv);

  /* Initialize bus and device of panel */
  panel->init();

  /* Start panel */
  panel->begin();

  /* Create a task to run the LVGL task periodically */
  lvgl_mux = xSemaphoreCreateRecursiveMutex();
  xTaskCreate(lvgl_port_task, "lvgl", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);

  /* Lock the mutex due to the LVGL APIs are not thread-safe */
  lvgl_port_lock(-1);

  ui_init();

  /* Release the mutex */
  lvgl_port_unlock();

  // Attach event handlers
  lv_obj_add_event_cb(ui_Button1, button_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_Button2, button_event_cb, LV_EVENT_CLICKED, NULL);

  // connecting to Wifi
  int nWifiConnectionTrials = 0;
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
  }
}

// Event handler for button clicks
static void button_event_cb(lv_event_t *e)
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
}

void loop()
{
  // Handle LVGL tasks
  lv_task_handler();
  delay(LOOP_DELAY_MS);
  
  client.poll();

  // reconnect wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wifi connection lost");
    ESP.restart();
  }
}


// decode base64 encoded png image to ps_ram
// https://github.com/OXRS-IO/OXRS-IO-TouchPanel-ESP32-FW
lv_img_dsc_t *decodeBase64ToImg(const char *imageBase64)
{
  // exit if no data to decode
  if (strlen(imageBase64) == 0)
    return NULL;

  size_t outLen = base64::decodeLength(imageBase64);
  uint8_t *raw = (uint8_t *)ps_malloc(outLen);
  base64::decode(imageBase64, raw);

  // prepaare image descriptor
  lv_img_dsc_t *imgPng = (lv_img_dsc_t *)ps_malloc(sizeof(lv_img_dsc_t));
  imgPng->header.cf = LV_IMG_CF_RAW_ALPHA;
  imgPng->header.always_zero = 0;
  imgPng->header.reserved = 0;
  imgPng->header.w = 0;
  imgPng->header.h = 0;
  imgPng->data_size = outLen;
  imgPng->data = raw;

  return imgPng;
}



void displayImage(lv_obj_t *parent, const char *base64Image) {
    lv_img_dsc_t *imgDsc = decodeBase64ToImg(base64Image);  
    if (imgDsc == NULL) return;

    lv_obj_t *img = lv_img_create(parent);
    lv_img_set_src(img, imgDsc);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    // You might need to handle memory management (freeing old images etc.)
}


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