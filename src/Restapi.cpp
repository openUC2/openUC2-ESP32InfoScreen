#include "RestApi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "uc2ui_controlpage.h"
#include <WebSocketsClient.h>

namespace RestApi
{
    static String _url;
    uint16_t websocket_server_port = 80;
    static WebSocketsClient client;
    bool socketConnected = false;

    void loop()
    {
        client.loop();
        delay(1);
    }

    void sendPostRequest(const String &endpoint, const JsonDocument &jsonDoc)
    {
        HTTPClient http;
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
        HTTPClient http;
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
        if (doc["modules"].containsKey("motor"))
            uc2ui_controlpage::setMotorModule(true);
        if (doc["modules"].containsKey("led"))
            uc2ui_controlpage::setLedModule(true);
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
                    uc2ui_controlpage::setMotorA(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 1 && doc["motor"]["steppers"][i]["isActivated"]) // X
                    uc2ui_controlpage::setMotorX(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 2 && doc["motor"]["steppers"][i]["isActivated"]) // Y
                    uc2ui_controlpage::setMotorY(true);
                if (doc["motor"]["steppers"][i]["stepperid"] == 3 && doc["motor"]["steppers"][i]["isActivated"]) // Z
                    uc2ui_controlpage::setMotorZ(true);
            }
        }
        doc.clear();
    }

    void getLed()
    {
        DynamicJsonDocument doc(512);
        sendGetRequest("/ledarr_get", doc);
        if (doc["ledArrNum"] != NULL)
            uc2ui_controlpage::setLedCount(doc["ledArrNum"]);
        if (doc["led_ison"] != NULL)
            uc2ui_controlpage::setLedOn(doc["led_ison"]);
        doc.clear();
    }

    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
    {

        switch (type)
        {
        case WStype_DISCONNECTED:
            log_i("[WSc] Disconnected!\n");
            socketConnected = false;
            break;
        case WStype_CONNECTED:
            log_i("[WSc] Connected to url: %s\n", payload);
            socketConnected = true;
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
        DynamicJsonDocument doc(512);
        doc["led"]["LEDArrMode"] = 1;
        doc["led"]["led_array"][0]["id"] = 0;
        doc["led"]["led_array"][0]["r"] = r;
        doc["led"]["led_array"][0]["g"] = g;
        doc["led"]["led_array"][0]["b"] = b;
        send_websocket_msg(doc);
        doc.clear();
    }

    int speeds[] = {-160000, -80000, -8000, -4000, -2000, -1000, -500, -200, -100, -50, -20, -10, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 4000, 8000, 80000, 160000};

    //{ stepperid: 0, speed: a, isforever: af },
    void driveMotorForever(int motor, int speed)
    {
        log_i("drive motor %i", motor);
        DynamicJsonDocument doc(512);
        doc["motor"]["steppers"][0]["stepperid"] = motor;
        doc["motor"]["steppers"][0]["speed"] = speeds[speed];
        if (speeds[speed] == 0)
            doc["motor"]["steppers"][0]["isforever"] = 0;
        else
            doc["motor"]["steppers"][0]["isforever"] = 1;
        send_websocket_msg(doc);
        doc.clear();
    }

    void connectTo(String url)
    {
        _url = url;
        getModules();
        getMotor();
        getLed();
        websocket_connect();
    }

}