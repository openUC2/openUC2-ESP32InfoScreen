#pragma once
#include "lvgl.h"
#include "Arduino.h"

namespace uc2ui_custompage
{
    void setCustomModule(bool enable);
    void setCustomOn(bool on);
    void setCustomCount(int count);
    void setCustomValueChangedListener(void valueChangedListener(int r, int g, int b));
    void setenableCustomListner(void ledlistner(bool enable,int r, int g, int b));
    void initUI(lv_obj_t * container);
};