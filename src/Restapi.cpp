#include "RestApi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include "uc2ui_ledpage.h"
#include "uc2ui_laserpage.h"
#include "uc2ui_motorpage.h"
#include "uc2ui_controller.h"
#include "uc2ui_wifipage.h"

namespace RestApi
{
    typedef struct
    {
        int arrtype;
        int r, g, b;
    } update_led_t;

    typedef struct
    {
        int arrtype;
        int ch1, ch2, ch3;
    } update_laser_t;

    typedef struct
    {
        int motor, speed;
    } updateMotorForever_t;

    typedef struct
    {
        int speedX, speedY;
    } updateMotorXYForever_t;

    static String _url;
    const uint16_t websocket_server_port = 80;
    static WebSocketsClient client;
    static HTTPClient http;
    bool socketConnected = false;
    static QueueHandle_t updateLedColorQueue;
    static QueueHandle_t driveMotorForeverQueue;
    static QueueHandle_t driveMotorXYForeverQueue;
    static QueueHandle_t updateLaserValueQueue;
    const int QueueElementSize = 2;
    xTaskHandle xHandle;

    void loop()
    {
        client.loop();
        delay(1);
    }

    void sendPostRequest(const String &endpoint, const JsonDocument &jsonDoc)
    {
        http.begin(_url + endpoint);
        http.addHeader("Content-Type", "application/json");

        String payload;
        serializeJson(jsonDoc, payload);

        int httpResponseCode = http.POST(payload);
        log_i("Sending POST to %s%s", _url, endpoint);
        http.end();
    }

    void sendGetRequest(String endpoint, JsonDocument &doc)
    {
        http.begin(_url + endpoint);
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            deserializeJson(doc, response);
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        log_i("Sending POST to %s%s", _url, endpoint);
        http.end();
    }

    void getModules()
    {
        DynamicJsonDocument doc(1024);
        sendGetRequest("/modules_get", doc);
        serializeJsonPretty(doc, Serial);
        if (doc["modules"].containsKey("motor"))
            uc2ui_motorpage::setMotorModule(true);
        if (doc["modules"].containsKey("led"))
            uc2ui_ledpage::setLedModule(true);
        if (doc["modules"].containsKey("laser"))
            uc2ui_laserpage::setLaserModule(true);
        doc.clear();
    }

    void getMotor()
    {
        DynamicJsonDocument doc(1024);
        sendGetRequest("/motor_get", doc);
        for (int i = 0; i < 4; i++)
        {
            if (doc["motor"]["steppers"][i] != NULL)
            {
                if (doc["motor"]["steppers"][i]["stepperid"] == 0 && doc["motor"]["steppers"][i]["isActivated"]) // A
                    uc2ui_motorpage::setMotorA(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 1 && doc["motor"]["steppers"][i]["isActivated"]) // X
                    uc2ui_motorpage::setMotorX(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 2 && doc["motor"]["steppers"][i]["isActivated"]) // Y
                    uc2ui_motorpage::setMotorY(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 3 && doc["motor"]["steppers"][i]["isActivated"]) // Z
                    uc2ui_motorpage::setMotorZ(true);
            }
        }
        doc.clear();
    }

    void getLed()
    {
        DynamicJsonDocument doc(512);
        sendGetRequest("/ledarr_get", doc);
        if (doc["ledArrNum"] != NULL)
            uc2ui_ledpage::setLedCount(doc["ledArrNum"]);
        if (doc["led_ison"] != NULL)
            uc2ui_ledpage::setLedOn(doc["led_ison"]);
        doc.clear();
    }

    void getLaser()
    {
        // e.g. {"task": "/laser_get", "LASERid":1, "LASERval": 51}
        DynamicJsonDocument doc(512);
        sendGetRequest("/laser_get", doc);
        /* TODO: anything to get here?
        if (doc["LASERid"] != NULL)
            uc2ui_ledpage::setLedCount(doc["lArrNum"]);
        if (doc["LASERval"] != NULL)
            uc2ui_ledpage::setLedOn(doc["laser_ison"]);
        */
        doc.clear();
    }

    void disconnect()
    {
        client.disconnect();
        uc2ui_controller::showMicroscopePage(false);
        uc2ui_wifipage::clearDevices();
    }

    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
    {

        switch (type)
        {
        case WStype_DISCONNECTED:
            log_i("[WSc] Disconnected!\n");
            socketConnected = false;
            disconnect();
            break;
        case WStype_CONNECTED:
            log_i("[WSc] Connected to url: %s\n", payload);
            socketConnected = true;
            uc2ui_controller::showMicroscopePage(true);
            break;
        case WStype_TEXT:
            log_i("[WSc] get text: %s\n", payload);

            // send message to server
            // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            log_i("[WSc] get binary length: %u\n", length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
        case WStype_ERROR:
            disconnect();
            break;
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
        }
    }

    void websocket_connect()
    {
        if (socketConnected)
            return;
        log_i("connect to websocket");
        // register callbacks and start the websocket client for frame transfer

        int nConnectionTrial = 0;
        String s = _url;
        s.replace("http://", "");
        client.begin(s, websocket_server_port, "/ws");
        client.onEvent(webSocketEvent);
        client.setReconnectInterval(5000);
    }

    void send_websocket_msg(JsonDocument &doc)
    {
        if (socketConnected)
        {
            String payload;
            serializeJson(doc, payload);
            client.sendTXT(payload);
        }
    }

    //{ led: { LEDArrMode: 1, led_array: [{ id: 0, b: bluec, r: redc, g: greenc }] } }
    void websocket_updateColors(int r, int g, int b)
    {
        log_i("websocket_updateColors");
        update_led_t led;
        led.r = r;
        led.g = g;
        led.b = b;
        int ret = xQueueSend(updateLedColorQueue, (void *)&led, 0);
    }

    void setLedOn(bool enable, int r, int g, int b)
    {
        DynamicJsonDocument doc(512);
        doc["led"]["LEDArrMode"] = enable ? 1 : 3;
        doc["led"]["led_array"][0]["id"] = 0;
        doc["led"]["led_array"][0]["r"] = r;
        doc["led"]["led_array"][0]["g"] = g;
        doc["led"]["led_array"][0]["b"] = b;
        send_websocket_msg(doc);
        // sendPostRequest("/ledarr_act", doc);
        doc.clear();
    }

    //{ led: { LEDArrMode: 1, led_array: [{ id: 0, b: bluec, r: redc, g: greenc }] } }
    void websocket_updateLaserValues(int ch1, int ch2, int ch3)
    {
        log_i("websocket_updateLaserValues");
        update_laser_t laser;
        laser.ch1 = ch1;
        laser.ch2 = ch2;
        laser.ch3 = ch3;
        int ret = xQueueSend(updateLaserValueQueue, (void *)&laser, 0);
    }

    void setLaserOn(bool enable, int ch1, int ch2, int ch3)
    {
        // {"task": "/laser_act", "LASERid":1, "LASERval": 51}
        DynamicJsonDocument doc(512);
        doc["laser"][0]["ch1"] = ch1;
        doc["laser"][0]["ch2"] = ch2;
        doc["laser"][0]["ch3"] = ch3;
        send_websocket_msg(doc);
        // sendPostRequest("/ledarr_act", doc);
        doc.clear();
    }

    int speeds[] = {-80000, -40000, -8000, -4000, -2000, -1000, -500, -200, -100, -50, -20, -10, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 4000, 8000, 40000, 80000};

    void driveMotorForever(JsonDocument &doc, int motor, int speed, int arrid)
    {
        doc["motor"]["steppers"][arrid]["stepperid"] = motor;
        doc["motor"]["steppers"][arrid]["speed"] = speed;
        if (speed == 0)
            doc["motor"]["steppers"][arrid]["isforever"] = 0;
        else
            doc["motor"]["steppers"][arrid]["isforever"] = 1;
    }

    //{ stepperid: 0, speed: a, isforever: af },
    void driveMotorForever(int motor, int speed)
    {
        log_i("drive motor %i", motor);
        updateMotorForever_t m;

        if (uxQueueMessagesWaiting(driveMotorForeverQueue) == QueueElementSize - 1)
            xQueueReceive(driveMotorForeverQueue, (void *)&m, 0);
        m.speed = speeds[speed];
        m.motor = motor;
        int ret = xQueueSend(driveMotorForeverQueue, (void *)&m, 0);
    }

    void driveMotorXYForever(int speedX, int speedY)
    {
        log_i("drive XY motor speed x:%i speed y:%i", speedX, speedY);
        updateMotorXYForever_t o;

        if (uxQueueMessagesWaiting(driveMotorXYForeverQueue) == QueueElementSize - 1)
            xQueueReceive(driveMotorXYForeverQueue, (void *)&o, 0);
        o.speedX = speedX;
        o.speedY = speedY;
        int ret = xQueueSend(driveMotorXYForeverQueue, (void *)&o, 0);
    }

    void sendSocketMsg(void *pvParameters)
    {
        for (;;)
        { // A Task shall never return or exit.
            // One approach would be to poll the function (uxQueueMessagesWaiting(QueueHandle) and call delay if nothing is waiting.
            // The other approach is to use infinite time to wait defined by constant `portMAX_DELAY`:
            if (updateLedColorQueue != NULL && uxQueueMessagesWaiting(updateLedColorQueue) > 0)
            { // Sanity check just to make sure the queue actually exists
                update_led_t led;
                BaseType_t ret = xQueueReceive(updateLedColorQueue, (void *)&led, 0);
                if (ret == pdTRUE)
                {
                    DynamicJsonDocument doc(512);
                    doc["led"]["LEDArrMode"] = 1;
                    doc["led"]["led_array"][0]["id"] = 0;
                    doc["led"]["led_array"][0]["r"] = led.r;
                    doc["led"]["led_array"][0]["g"] = led.g;
                    doc["led"]["led_array"][0]["b"] = led.b;
                    send_websocket_msg(doc);
                    doc.clear();
                }
            }
            if (updateLaserValueQueue != NULL && uxQueueMessagesWaiting(updateLaserValueQueue) > 0)
            { // Sanity check just to make sure the queue actually exists
                update_laser_t laser;
                BaseType_t ret = xQueueReceive(updateLaserValueQueue, (void *)&laser, 0);
                if (ret == pdTRUE)
                {
                    DynamicJsonDocument doc(512);
                    doc["laser"]["ch1"] = laser.ch1;
                    doc["laser"]["ch2"] = laser.ch2;
                    doc["laser"]["ch3"] = laser.ch3;
                    send_websocket_msg(doc);
                    doc.clear();
                }
            }            
            if (driveMotorForeverQueue != NULL && uxQueueMessagesWaiting(driveMotorForeverQueue) > 0)
            { // Sanity check just to make sure the queue actually exists
                updateMotorForever_t m;
                BaseType_t ret = xQueueReceive(driveMotorForeverQueue, &m, 0);
                if (ret == pdTRUE)
                {
                    DynamicJsonDocument doc(512);
                    driveMotorForever(doc, m.motor, m.speed, 0);
                    send_websocket_msg(doc);
                    doc.clear();
                }
            }
            if (driveMotorXYForeverQueue != NULL && uxQueueMessagesWaiting(driveMotorXYForeverQueue) > 0)
            { // Sanity check just to make sure the queue actually exists
                updateMotorXYForever_t m;
                int ret = xQueueReceive(driveMotorXYForeverQueue, &m, 0);
                if (ret == pdTRUE)
                {
                    DynamicJsonDocument doc(512);
                    driveMotorForever(doc, 1, m.speedX, 0);
                    driveMotorForever(doc, 2, m.speedY, 1);
                    send_websocket_msg(doc);
                    doc.clear();
                }
            }
            delay(1); // Sanity check
        }             // Infinite loop
    }

    void connectTo(String url)
    {
        _url = url;
        getModules();
        getMotor();
        getLed();
        getLaser();
        websocket_connect();
        if (xHandle != nullptr)
            vTaskDelete(xHandle);
        xTaskCreate(sendSocketMsg, "sendsocketmsg", 4 * 1024, NULL, 1, &xHandle);
    }

    void init()
    {
        updateLedColorQueue = xQueueCreate(QueueElementSize, sizeof(update_led_t));
        if (updateLedColorQueue == 0)
            log_e("Failes to create json queue");
        updateLaserValueQueue = xQueueCreate(QueueElementSize, sizeof(update_laser_t));
        if (updateLaserValueQueue == 0)
            log_e("failed to crete json queue (laser)");
        driveMotorForeverQueue = xQueueCreate(QueueElementSize, sizeof(updateMotorForever_t));
        if (driveMotorForeverQueue == 0)
            log_e("Failes to create json queue");
        driveMotorXYForeverQueue = xQueueCreate(QueueElementSize, sizeof(updateMotorXYForever_t));
        if (driveMotorXYForeverQueue == 0)
            log_e("Failes to create json queue");
    }

}