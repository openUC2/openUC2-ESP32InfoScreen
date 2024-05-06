#pragma once
#include "lvgl.h"
#include "Arduino.h"

namespace uc2ui_custompage
{
    void setCustomModule(bool enable);
    void setCustomOn(bool on);
    void setCustomCount(int count);
    void setCustomValueChangedListener(void valueChangedListener(char *topic, int value));
    void initUI(lv_obj_t * container);
};