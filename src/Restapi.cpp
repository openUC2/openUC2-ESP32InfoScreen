#include "RestApi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "uc2ui_controlpage.h"
#include <ArduinoWebsockets.h>

namespace RestApi
{
    static String _url;
    uint16_t websocket_server_port = 80;
    using namespace websockets;
    WebsocketsClient client;
    bool socketConnected = false;

    void connectTo(String url)
    {
        _url = url;
        getModules();
        getMotor();
        getLed();
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
        sendGetRequest("/led_get", doc);
        if (doc["ledArrNum"] != NULL)
            uc2ui_controlpage::setLedCount(doc["ledArrNum"]);
        if (doc["led_ison"] != NULL)
            uc2ui_controlpage::setLedOn(doc["led_ison"]);
        doc.clear();
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

    void websocket_connect()
    {
        if (socketConnected)
            return;

        // register callbacks and start the websocket client for frame transfer
        client.onMessage(onMessageCallback);
        client.onEvent(onEventsCallback);
        int nConnectionTrial = 0;
        while (!client.connect(_url, websocket_server_port, "/ws") && nConnectionTrial < 10)
        {
            nConnectionTrial++;
            delay(500);
        }
        if(nConnectionTrial == 10)
            log_e("socket connection to %s failed!", _url);
    }

    void send_websocket_msg(JsonDocument doc)
    {
        if (socketConnected)
        {
            String payload;
            serializeJson(doc, payload);
            client.stream(payload);
        }
    }
}