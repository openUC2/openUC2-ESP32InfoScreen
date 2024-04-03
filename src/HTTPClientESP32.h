#ifndef HTTPClientESP32_h
#define HTTPClientESP32_h

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class HTTPClientESP32 {
public:
    HTTPClientESP32(const String& baseURL);
    void sendPostRequest(const String& endpoint, const JsonDocument& jsonDoc);
    void sendPostRequest(const String& endpoint, const String payload);
    void motor_act(int stepperid, int position, int speed, int isabs, int isaccel);
    void laser_act(int laserid, int state);
    
private:
    String _ssid;
    String _password;
    String _baseURL;
};

#endif
