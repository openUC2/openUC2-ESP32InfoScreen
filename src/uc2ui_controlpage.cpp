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

    lv_obj_t *container1;
    lv_obj_t *container2;

    lv_obj_t *TextAreaURL;
    lv_obj_t *ButtonConnectToHost;
    lv_obj_t *LabelConnectToHost;

    lv_obj_t *motorPanel;
    lv_obj_t *LabelMotor;
    lv_obj_t *motorASlider;
    lv_obj_t *motorXSlider;
    lv_obj_t *motorYSlider;
    lv_obj_t *motorZSlider;
    lv_obj_t *LabelA;
    lv_obj_t *LabelX;
    lv_obj_t *LabelY;
    lv_obj_t *LabelZ;

    lv_obj_t *ledPanel;
    lv_obj_t *LedLabel;
    lv_obj_t *LedOnCheckBox;
    lv_obj_t *LabelBlue;
    lv_obj_t *LabelGreen;
    lv_obj_t *LabelRed;
    lv_obj_t *SliderBlue;
    lv_obj_t *SliderGreen;
    lv_obj_t *SliderRed;

    void (*updatedColorsListner)(int r, int g, int b);

    void setColorChangedListner(void colorchangedlistner(int r, int g, int b))
    {
        updatedColorsListner = colorchangedlistner;
    }

    void (*enableLedListner)(bool enable, int r, int g, int b);

    void setenableLedListner(void ledlistner(bool enable, int r, int g, int b))
    {
        enableLedListner = ledlistner;
    }

    void (*updateMotorSpeedListner)(int motor, int speed);

    void setUpdateMotorSpeedListner(void updateMotorSpeed(int motor, int speed))
    {
        updateMotorSpeedListner = updateMotorSpeed;
    }

    void (*connectToHost)(String s);

    void setConnectToHostListner(void contoHst(String s))
    {
        connectToHost = contoHst;
    }

    // focus event for textareas to trigger the keyboard visibility
    void (*fev)(lv_event_t *ob);

    void setVisibility(lv_obj_t *ob, bool vis)
    {
        if (vis)
            lv_obj_clear_flag(ob, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(ob, LV_OBJ_FLAG_HIDDEN);
        lv_obj_update_layout(ob);
    }

    void setLedModule(bool enable)
    {
        led_module = enable;
        setVisibility(ledPanel, enable);
    }

    void setMotorModule(bool enable)
    {
        motor_module = enable;
        setVisibility(motorPanel, enable);
    }

    void setMotorX(bool enable)
    {
        x_motor = enable;
        setVisibility(motorXSlider, enable);
        setVisibility(LabelX, enable);
    }

    void setMotorY(bool enable)
    {
        y_motor = enable;
        setVisibility(motorYSlider, enable);
        setVisibility(LabelY, enable);
    }

    void setMotorZ(bool enable)
    {
        z_motor = enable;
        setVisibility(motorZSlider, enable);
        setVisibility(LabelZ, enable);
    }

    void setMotorA(bool enable)
    {
        a_motor = enable;
        setVisibility(motorASlider, enable);
        setVisibility(LabelA, enable);
    }

    void showConnect(bool show)
    {
        setVisibility(ButtonConnectToHost, show);
        setVisibility(TextAreaURL, show);
        if (show)
        {
            setVisibility(motorPanel, false);
            setVisibility(ledPanel, false);
        }
        else
        {
            setVisibility(motorPanel, motor_module);
            setVisibility(ledPanel, led_module);
        }
        log_i("show modules motor:%i, led:%i", motor_module, led_module);
    }

    void setLedOn(bool on)
    {
        led_on = on;
        if (on)
            lv_obj_add_state(LedOnCheckBox, LV_STATE_CHECKED);
        else
            lv_obj_clear_state(LedOnCheckBox, LV_STATE_CHECKED);
    }

    void setLedCount(int count)
    {
        led_count = count;
    }

    void hideAllMotorViews()
    {
        setVisibility(motorASlider, false);
        setVisibility(motorXSlider, false);
        setVisibility(motorYSlider, false);
        setVisibility(motorZSlider, false);

        setVisibility(LabelA, false);
        setVisibility(LabelX, false);
        setVisibility(LabelY, false);
        setVisibility(LabelZ, false);
    }

    void onColorSliderChanged(lv_event_t *e)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderRed);
            int g = lv_slider_get_value(SliderGreen);
            int b = lv_slider_get_value(SliderBlue);
            if (updatedColorsListner != nullptr)
                updatedColorsListner(r, g, b);
        }
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

    void button_event_cb(lv_event_t *e)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        if (obj == ButtonConnectToHost)
        {
            const char *s = lv_textarea_get_text(TextAreaURL);
            String str = "http://";
            str += s;
            log_i("Conntect to Host:%s", str.c_str());
            connectToHost(str);
        }
    }

    void ledCheckBox_event_handler(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderRed);
            int g = lv_slider_get_value(SliderGreen);
            int b = lv_slider_get_value(SliderBlue);
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            if (enableLedListner != nullptr)
                enableLedListner(state, r, g, b);
        }
    }

    void initMotorPanel()
    {
        motorASlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorASlider, 20);
        lv_obj_set_height(motorASlider, 266);
        lv_obj_set_x(motorASlider, -130);
        lv_obj_set_y(motorASlider, 40);
        lv_obj_set_align(motorASlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorASlider, 0, 35);
        lv_slider_set_value(motorASlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorASlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorASlider, 0, LV_ANIM_OFF);

        motorXSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorXSlider, 20);
        lv_obj_set_height(motorXSlider, 266);
        lv_obj_set_x(motorXSlider, -50);
        lv_obj_set_y(motorXSlider, 40);
        lv_obj_set_align(motorXSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorXSlider, 0, 35);
        lv_slider_set_value(motorXSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorXSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorXSlider, 0, LV_ANIM_OFF);

        motorYSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorYSlider, 20);
        lv_obj_set_height(motorYSlider, 266);
        lv_obj_set_x(motorYSlider, 50);
        lv_obj_set_y(motorYSlider, 40);
        lv_obj_set_align(motorYSlider, LV_ALIGN_TOP_MID);
        lv_slider_set_range(motorYSlider, 0, 35);
        lv_slider_set_value(motorYSlider, 17, LV_ANIM_OFF);
        if (lv_slider_get_mode(motorYSlider) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(motorYSlider, 0, LV_ANIM_OFF);

        motorZSlider = lv_slider_create(motorPanel);
        lv_obj_set_width(motorZSlider, 20);
        lv_obj_set_height(motorZSlider, 266);
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

        LabelMotor = lv_label_create(motorPanel);
        lv_obj_set_width(LabelMotor, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelMotor, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelMotor, 0);
        lv_obj_set_y(LabelMotor, 0);
        lv_obj_set_align(LabelMotor, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelMotor, "Motor");

        lv_obj_add_event_cb(motorASlider, onMotorAchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorXSlider, onMotorXchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorYSlider, onMotorYchanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(motorZSlider, onMotorZchanged, LV_EVENT_ALL, NULL);

        // hideAllMotorViews();
    }

    void initLedPanel()
    {
        LedLabel = lv_label_create(ledPanel);
        lv_obj_set_width(LedLabel, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LedLabel, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LedLabel, 0);
        lv_obj_set_y(LedLabel, 0);
        lv_obj_set_align(LedLabel, LV_ALIGN_TOP_MID);
        lv_label_set_text(LedLabel, "Led");

        LedOnCheckBox = lv_checkbox_create(ledPanel);
        lv_checkbox_set_text(LedOnCheckBox, "Led On");
        lv_obj_set_width(LedOnCheckBox, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LedOnCheckBox, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LedOnCheckBox, -130);
        lv_obj_set_y(LedOnCheckBox, 30);
        lv_obj_set_align(LedOnCheckBox, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(LedOnCheckBox, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

        LabelBlue = lv_label_create(ledPanel);
        lv_obj_set_width(LabelBlue, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelBlue, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelBlue, 0);
        lv_obj_set_y(LabelBlue, 170);
        lv_obj_set_align(LabelBlue, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelBlue, "Blue");

        LabelGreen = lv_label_create(ledPanel);
        lv_obj_set_width(LabelGreen, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelGreen, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelGreen, 0);
        lv_obj_set_y(LabelGreen, 120);
        lv_obj_set_align(LabelGreen, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelGreen, "Green");

        LabelRed = lv_label_create(ledPanel);
        lv_obj_set_width(LabelRed, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelRed, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelRed, 0);
        lv_obj_set_y(LabelRed, 70);
        lv_obj_set_align(LabelRed, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelRed, "Red");

        SliderBlue = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderBlue, 0, 255);
        lv_slider_set_value(SliderBlue, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderBlue) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderBlue, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderBlue, 280);
        lv_obj_set_height(SliderBlue, 20);
        lv_obj_set_x(SliderBlue, 0);
        lv_obj_set_y(SliderBlue, 190);
        lv_obj_set_align(SliderBlue, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        SliderGreen = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderGreen, 0, 255);
        lv_slider_set_value(SliderGreen, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderGreen) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderGreen, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderGreen, 280);
        lv_obj_set_height(SliderGreen, 20);
        lv_obj_set_x(SliderGreen, 0);
        lv_obj_set_y(SliderGreen, 140);
        lv_obj_set_align(SliderGreen, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        SliderRed = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderRed, 0, 255);
        lv_slider_set_value(SliderRed, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderRed) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderRed, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderRed, 280);
        lv_obj_set_height(SliderRed, 20);
        lv_obj_set_x(SliderRed, 0);
        lv_obj_set_y(SliderRed, 90);
        lv_obj_set_align(SliderRed, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(SliderRed, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderGreen, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderBlue, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(LedOnCheckBox, ledCheckBox_event_handler, LV_EVENT_ALL, NULL);
    }

    void uiInit(lv_obj_t *controlPage, void func(lv_event_t *ob))
    {

        fev = func;
        lv_obj_set_flex_flow(controlPage, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(controlPage, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        container2 = lv_obj_create(controlPage);
        lv_obj_remove_style_all(container2);
        lv_obj_set_width(container2, LV_SIZE_CONTENT);  /// 100
        lv_obj_set_height(container2, LV_SIZE_CONTENT); /// 50
        lv_obj_set_x(container2, 0);
        lv_obj_set_y(container2, -16);
        lv_obj_set_align(container2, LV_ALIGN_CENTER);
        lv_obj_set_flex_flow(container2, LV_FLEX_FLOW_ROW_REVERSE);
        lv_obj_set_flex_align(container2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(container2, LV_OBJ_FLAG_SCROLLABLE);

        TextAreaURL = lv_textarea_create(container2);
        lv_obj_set_width(TextAreaURL, 321);
        lv_obj_set_height(TextAreaURL, LV_SIZE_CONTENT);
        lv_textarea_set_placeholder_text(TextAreaURL, "URL...");
        lv_textarea_set_one_line(TextAreaURL, true);
        lv_obj_add_event_cb(TextAreaURL, fev, LV_EVENT_ALL, NULL);
        lv_textarea_set_text(TextAreaURL, "192.168.4.1");

        ButtonConnectToHost = lv_btn_create(container2);
        lv_obj_set_width(ButtonConnectToHost, 100);
        lv_obj_set_height(ButtonConnectToHost, 45);
        lv_obj_set_x(ButtonConnectToHost, 0);
        lv_obj_set_y(ButtonConnectToHost, -16);
        lv_obj_set_align(ButtonConnectToHost, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(ButtonConnectToHost, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_clear_flag(ButtonConnectToHost, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_add_event_cb(ButtonConnectToHost, button_event_cb, LV_EVENT_CLICKED, NULL);

        LabelConnectToHost = lv_label_create(ButtonConnectToHost);
        lv_obj_set_width(LabelConnectToHost, LV_SIZE_CONTENT);
        lv_obj_set_height(LabelConnectToHost, LV_SIZE_CONTENT);
        lv_obj_set_align(LabelConnectToHost, LV_ALIGN_CENTER);
        lv_label_set_text(LabelConnectToHost, "Connect");

        container1 = lv_obj_create(controlPage);
        lv_obj_remove_style_all(container1);
        lv_obj_set_width(container1, lv_pct(100));
        lv_obj_set_height(container1, lv_pct(92));
        lv_obj_set_x(container1, 0);
        lv_obj_set_y(container1, 35);
        lv_obj_set_align(container1, LV_ALIGN_TOP_MID);
        lv_obj_set_flex_flow(container1, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(container1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(container1, LV_OBJ_FLAG_SCROLLABLE);

        motorPanel = lv_obj_create(container1);
        lv_obj_set_width(motorPanel, lv_pct(50));
        lv_obj_set_height(motorPanel, lv_pct(100));
        lv_obj_set_x(motorPanel, -299);
        lv_obj_set_y(motorPanel, -19);
        lv_obj_clear_flag(motorPanel, LV_OBJ_FLAG_SCROLLABLE);
        setVisibility(motorPanel, false);

        initMotorPanel();

        ledPanel = lv_obj_create(container1);
        lv_obj_set_width(ledPanel, lv_pct(49));
        lv_obj_set_height(ledPanel, lv_pct(100));
        lv_obj_set_x(ledPanel, 137);
        lv_obj_set_y(ledPanel, 59);
        lv_obj_clear_flag(ledPanel, LV_OBJ_FLAG_SCROLLABLE);
        setVisibility(ledPanel, false);

        initLedPanel();
    }
}