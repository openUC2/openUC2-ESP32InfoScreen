#include "uc2ui_motoradvpage.h"
#include "Arduino.h"

namespace uc2ui_motoradvpage
{
    lv_obj_t *joyContainer;
    lv_obj_t *joyButton;
    const int centerx = 400;
    const int centery = 240;
    const int halfsizecontainer = 150;
    int last_x_speed = 0;
    int last_y_speed = 0;

    void (*driveXYMotor)(int speedX, int speedY);

    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY))
    {
        driveXYMotor = driveXYMotorListner;
    }

    void joyButtonEventListner(lv_event_t *e)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_PRESSING)
        {
            lv_indev_t *indev = lv_indev_get_act();
            lv_point_t p;
            lv_indev_get_point(indev, &p);

            int transXtoCenter = p.x - centerx;
            int transYtoCenter = p.y - centery;
            if (transXtoCenter > halfsizecontainer - 30)
                transXtoCenter = halfsizecontainer - 30;
            if (transXtoCenter < -(halfsizecontainer - 30))
                transXtoCenter = -(halfsizecontainer - 30);
            if (transYtoCenter > halfsizecontainer - 30)
                transYtoCenter = halfsizecontainer - 30;
            if (transYtoCenter < -(halfsizecontainer - 30))
                transYtoCenter = -(halfsizecontainer - 30);

            lv_obj_set_x(joyButton, transXtoCenter);
            lv_obj_set_y(joyButton, transYtoCenter);
            if (driveXYMotor != nullptr)
            {
                int x = transXtoCenter * 10;
                int y = transYtoCenter * 10;
                if (last_x_speed != x && last_y_speed != y)
                {
                    last_x_speed = x;
                    last_y_speed = y;
                    driveXYMotor(transXtoCenter * 10, transYtoCenter * 10);
                }
            }
        }
        if (event_code == LV_EVENT_RELEASED)
        {
            lv_obj_set_x(joyButton, 0);
            lv_obj_set_y(joyButton, 0);
            last_x_speed = 0;
            last_y_speed = 0;
            if (driveXYMotor != nullptr)
                driveXYMotor(0, 0);
        }
    }

    void initUi(lv_obj_t *motoradvpage)
    {
        joyContainer = lv_obj_create(motoradvpage);
        lv_obj_remove_style_all(joyContainer);
        lv_obj_set_width(joyContainer, 300);
        lv_obj_set_height(joyContainer, 300);
        lv_obj_set_align(joyContainer, LV_ALIGN_CENTER);
        lv_obj_clear_flag(joyContainer, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(joyContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0x7E7E7E), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

        joyButton = lv_btn_create(joyContainer);
        lv_obj_set_width(joyButton, 60);
        lv_obj_set_height(joyButton, 60);
        lv_obj_set_align(joyButton, LV_ALIGN_CENTER);
        lv_obj_set_x(joyButton, 0);
        lv_obj_set_y(joyButton, 0);
        lv_obj_add_flag(joyButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(joyButton, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(joyButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(joyButton, joyButtonEventListner, LV_EVENT_ALL, NULL);
    }
}