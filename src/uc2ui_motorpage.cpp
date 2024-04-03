#include "uc2ui_motorpage.h"
#include "lvgl_helper.h"

namespace uc2ui_motorpage
{

    bool motor_module = false;
    bool a_motor = false;
    bool x_motor = false;
    bool y_motor = false;
    bool z_motor = false;
    lv_obj_t *motorPanel;;
    lv_obj_t *motorASlider;
    lv_obj_t *motorXSlider;
    lv_obj_t *motorYSlider;
    lv_obj_t *motorZSlider;
    lv_obj_t *LabelA;
    lv_obj_t *LabelX;
    lv_obj_t *LabelY;
    lv_obj_t *LabelZ;

    lv_obj_t *joyContainer;
    lv_obj_t *joyButton;
    const int centerx = 400;
    const int centery = 240;
    const int halfsizecontainer = 175;
    int last_x_speed = 0;
    int last_y_speed = 0;
    int joybutton_real_x_pos;

    void (*updateMotorSpeedListner)(int motor, int speed);

    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed))
    {
        updateMotorSpeedListner = updateMotorSpeed;
    }

    void (*driveXYMotor)(int speedX, int speedY);

    void setDriveXYMotorListner(void driveXYMotorListner(int speedX, int speedY))
    {
        driveXYMotor = driveXYMotorListner;
    }


    void onMotorSpeedChanged(lv_event_t *e, int motor)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_VALUE_CHANGED)
        {
            int speed = lv_slider_get_value(target);
            if (updateMotorSpeedListner != nullptr)
                updateMotorSpeedListner(motor, speed);
        }
        if (event_code == LV_EVENT_RELEASED)
        {
            lv_slider_set_value(target, 17, LV_ANIM_OFF);
            if (updateMotorSpeedListner != nullptr)
                updateMotorSpeedListner(motor, 17);
        }
    }

    void onMotorAchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 0);
    }

    void onMotorXchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 1);
    }

    void onMotorYchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 2);
    }

    void onMotorZchanged(lv_event_t *e)
    {
        onMotorSpeedChanged(e, 3);
    }

    void hideAllMotorViews()
    {
        lvgl_helper::setVisibility(motorASlider, false);
        lvgl_helper::setVisibility(motorXSlider, false);
        lvgl_helper::setVisibility(motorYSlider, false);
        lvgl_helper::setVisibility(motorZSlider, false);

        lvgl_helper::setVisibility(LabelA, false);
        lvgl_helper::setVisibility(LabelX, false);
        lvgl_helper::setVisibility(LabelY, false);
        lvgl_helper::setVisibility(LabelZ, false);
    }

    void setMotorModule(bool enable)
    {
        motor_module = enable;
        lvgl_helper::setVisibility(motorPanel, enable);
    }

    void setMotorX(bool enable)
    {
        x_motor = enable;
        lvgl_helper::setVisibility(motorXSlider, enable);
        lvgl_helper::setVisibility(LabelX, enable);
        if (y_motor && x_motor)
            lvgl_helper::setVisibility(joyContainer, enable);
    }

    void setMotorY(bool enable)
    {
        y_motor = enable;
        lvgl_helper::setVisibility(motorYSlider, enable);
        lvgl_helper::setVisibility(LabelY, enable);
        if (y_motor && x_motor)
            lvgl_helper::setVisibility(joyContainer, enable);
    }

    void setMotorZ(bool enable)
    {
        z_motor = enable;
        lvgl_helper::setVisibility(motorZSlider, enable);
        lvgl_helper::setVisibility(LabelZ, enable);
    }

    void setMotorA(bool enable)
    {
        a_motor = enable;
        lvgl_helper::setVisibility(motorASlider, enable);
        lvgl_helper::setVisibility(LabelA, enable);
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

            int transXtoCenter = p.x - (centerx +175);
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

    void initUI(lv_obj_t *container)
    {
        motorPanel = lv_obj_create(container);
        lv_obj_set_height(motorPanel, 350);
        lv_obj_set_width(motorPanel, lv_pct(50));
        lv_obj_clear_flag(motorPanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(motorPanel, false);

        motorASlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorASlider, 20);
        lv_obj_set_height(motorASlider, 250);
        lv_obj_set_x(motorASlider, -130);
        lv_obj_set_y(motorASlider, 40);
        lv_obj_set_align(motorASlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorASlider, 0, 35);
        lv_slider_set_value(motorASlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorASlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorASlider, 0, LV_ANIM_OFF);

        motorXSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorXSlider, 20);
        lv_obj_set_height(motorXSlider, 250);
        lv_obj_set_x(motorXSlider, -50);
        lv_obj_set_y(motorXSlider, 40);
        lv_obj_set_align(motorXSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorXSlider, 0, 35);
        lv_slider_set_value(motorXSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorXSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorXSlider, 0, LV_ANIM_OFF);

        motorYSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorYSlider, 20);
        lv_obj_set_height(motorYSlider, 250);
        lv_obj_set_x(motorYSlider, 50);
        lv_obj_set_y(motorYSlider, 40);
        lv_obj_set_align(motorYSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorYSlider, 0, 35);
        lv_slider_set_value(motorYSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorYSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorYSlider, 0, LV_ANIM_OFF);

        motorZSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorZSlider, 20);
        lv_obj_set_height(motorZSlider, 250);
        lv_obj_set_x(motorZSlider, 130);
        lv_obj_set_y(motorZSlider, 40);
        lv_obj_set_align(motorZSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorZSlider, 0, 35);
        lv_slider_set_value(motorZSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorZSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorZSlider, 0, LV_ANIM_OFF);

        LabelA = lv_label_create(motorPanel);
        lv_obj_set_width(LabelA, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelA, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelA, -130);
        lv_obj_set_y(LabelA, 10);
        lv_obj_set_align(LabelA, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelA, "A");

        LabelX = lv_label_create(motorPanel);
        lv_obj_set_width(LabelX, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelX, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelX, -50);
        lv_obj_set_y(LabelX, 10);
        lv_obj_set_align(LabelX, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelX, "X");

        LabelY = lv_label_create(motorPanel);
        lv_obj_set_width(LabelY, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelY, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelY, 50);
        lv_obj_set_y(LabelY, 10);
        lv_obj_set_align(LabelY, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelY, "Y");

        LabelZ = lv_label_create(motorPanel);
        lv_obj_set_width(LabelZ, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelZ, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelZ, 130);
        lv_obj_set_y(LabelZ, 10);
        lv_obj_set_align(LabelZ, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelZ, "Z");

        lv_obj_add_event_cb(motorASlider, onMotorAchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorXSlider, onMotorXchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorYSlider, onMotorYchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorZSlider, onMotorZchanged, LV_EVENT_ALL, NULL);

        joyContainer = lv_obj_create(container);
        lv_obj_remove_style_all(joyContainer);
        lv_obj_set_width(joyContainer, 350);
        lv_obj_set_height(joyContainer, 350);
        lv_obj_set_align(joyContainer, LV_ALIGN_TOP_RIGHT);
        lv_obj_clear_flag(joyContainer, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(joyContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0x7E7E7E), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(joyContainer, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyContainer, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

        joyButton = lv_btn_create(joyContainer);
        lv_obj_set_width(joyButton, 50);
        lv_obj_set_height(joyButton, 50);
        lv_obj_set_align(joyButton, LV_ALIGN_CENTER);
        lv_obj_set_x(joyButton, 0);
        lv_obj_set_y(joyButton, 0);

        lv_area_t  c;
        lv_obj_get_coords(joyButton,&c);
        joybutton_real_x_pos = c.x1 + (c.x1 - c.x2 / 2);
        //lv_obj_add_flag(joyButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(joyButton, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(joyButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(joyButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(joyButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(joyButton, joyButtonEventListner, LV_EVENT_ALL, NULL);

        // hideAllMotorViews();
    }
}