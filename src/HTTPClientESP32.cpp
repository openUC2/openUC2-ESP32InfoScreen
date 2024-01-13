#include "HTTPClientESP32.h"

HTTPClientESP32::HTTPClientESP32(const String& baseURL) 
    : _baseURL(baseURL) {}



void HTTPClientESP32::sendPostRequest(const String& endpoint, const JsonDocument& jsonDoc) {
    HTTPClient http;
    http.begin(_baseURL + endpoint); 
    http.addHeader("Content-Type", "application/json");

    String payload;
    serializeJson(jsonDoc, payload);

    int httpResponseCode = http.POST(payload);
    Serial.println("Sending POST to "+ _baseURL + endpoint);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void HTTPClientESP32::sendPostRequest(const String& endpoint, const String payload) {
    HTTPClient http;
    http.begin(_baseURL + endpoint); 
    http.addHeader("Content-Type", "application/json");


    int httpResponseCode = http.POST(payload);
    Serial.println("Sending POST to "+ _baseURL + endpoint);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}


void HTTPClientESP32::motor_act(int stepperid, int position, int speed, int isabs, int isaccel){
    // Prepare a JSON payload using ArduinoJson
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["motor"]["steppers"][0]["stepperid"] = stepperid;
    jsonDoc["motor"]["steppers"][0]["position"] = position;
    jsonDoc["motor"]["steppers"][0]["speed"] = speed;
    jsonDoc["motor"]["steppers"][0]["isabs"] = isabs;
    jsonDoc["motor"]["steppers"][0]["isaccel"] = isaccel;
    log_d("motor_act");
    // Send POST request
    sendPostRequest("motor_act", jsonDoc);

}