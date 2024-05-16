#pragma once
#include "lvgl.h"
#include "Arduino.h"

namespace uc2ui_messagepage
{
    void setMessageModule(bool enable);
    void setMessageOn(bool on);
    void setMessageCount(int count);
    void setMessageValueChangedListener(void valueChangedListener(char *topic, int value));
    void initUI(lv_obj_t * container);
};