#include "uc2ui_controlpage.h"

namespace uc2ui_controlpage
{
    static bool motor_module = false;
    static bool a_motor = false;
    static bool x_motor = false;
    static bool y_motor = false;
    static bool z_motor = false;
    static bool led_module = false;
    static int led_count;
    static bool led_on = false;

    void setLedModule(bool enable)
    {
        led_module = enable;
    }

    void setMotorModule(bool enable)
    {
        motor_module = enable;
    }

    void setMotorX(bool enable)
    {
        x_motor = enable;
    }

    void setMotorY(bool enable)
    {
        y_motor = enable;
    }

    void setMotorZ(bool enable)
    {
        z_motor = enable;
    }

    void setMotorA(bool enable)
    {
        z_motor = enable;
    }

    void setLedOn(bool on)
    {
        led_on = on;
    }
    void setLedCount(int count)
    {
        led_count = count;
    }

}