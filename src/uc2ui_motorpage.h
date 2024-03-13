#pragma once
#include "lvgl.h"
namespace uc2ui_motorpage
{
    void initUI(lv_obj_t *container);
    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY));
    void setMotorModule(bool enable);
    void setMotorX(bool enable);
    void setMotorY(bool enable);
    void setMotorZ(bool enable);
    void setMotorA(bool enable);
    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed));
}; // namespace uc2ui_motorpage
