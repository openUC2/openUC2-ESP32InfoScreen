#pragma once
#include "lvgl.h"

namespace uc2ui_ledpage
{
    void setLedModule(bool enable);
    void setLedOn(bool on);
    void setLedCount(int count);
    void setColorChangedListner(void colorchangedlistner(int r, int g, int b));
    void setenableLedListner(void ledlistner(bool enable,int r, int g, int b));
    void initUI(lv_obj_t * container);
};