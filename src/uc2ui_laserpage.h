#pragma once
#include "lvgl.h"

namespace uc2ui_laserpage
{
    void setLaserModule(bool enable);
    void setLaserOn(bool on);
    void setLaserCount(int count);
    void setLaserValueChangedListener(void valueChangedListener(int r, int g, int b));
    void setenableLaserListner(void ledlistner(bool enable,int r, int g, int b));
    void initUI(lv_obj_t * container);
};