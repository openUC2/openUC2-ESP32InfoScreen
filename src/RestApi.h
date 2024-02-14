#pragma once
#include "Arduino.h"
namespace RestApi
{
    void connectTo(String url);
    void websocket_updateColors(int r, int g, int b);
    void driveMotorForever(int motor, int speed);
    void loop();
};