#pragma once
#include "lvgl.h"
#include "Arduino.h"

namespace uc2ui_controlpage
{
    void setLedModule(bool enable);
    void setMotorModule(bool enable);
    void setMotorX(bool enable);
    void setMotorY(bool enable);
    void setMotorZ(bool enable);
    void setMotorA(bool enable);
    void setLedOn(bool on);
    void setLedCount(int count);
    void setLaserOn(bool on);
    void setLaserValue(int value);
    void uiInit(lv_obj_t * controlPage,void func(lv_event_t *ob));
    void setColorChangedListner(void colorchangedlistner(int r, int g, int b));
    void setConnectToHostListner(void contoHst(String s));
    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed));
    void setenableLedListner(void ledlistner(bool enable,int r, int g, int b));
    void setenableLaserListner(void laserlistner(bool enable, int value));
    void showConnect(bool show);
};