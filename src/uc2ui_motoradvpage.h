#pragma once
#include "lvgl.h"

namespace uc2ui_motoradvpage
{
    void initUi(lv_obj_t * motoradvpage);
    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY));
}; // namespace uc2ui_motoradvpage
