#include "uc2ui_ledpage.h"
#include "lvgl_helper.h"

namespace uc2ui_ledpage
{
    bool led_module = false;
    int led_count;
    bool led_on = false;

    lv_obj_t *ledPanel;
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

    void setLedModule(bool enable)
    {
        led_module = enable;
        lvgl_helper::setVisibility(ledPanel, enable);
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

    void initLedPanel()
    {

        LedOnCheckBox = lv_checkbox_create(ledPanel);
        lv_checkbox_set_text(LedOnCheckBox, "Led On");
        lv_obj_set_width(LedOnCheckBox, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LedOnCheckBox, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LedOnCheckBox, -130);
        lv_obj_set_y(LedOnCheckBox, 30);
        lv_obj_set_align(LedOnCheckBox, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(LedOnCheckBox, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

        LabelRed = lv_label_create(ledPanel);
        lv_obj_set_width(LabelRed, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelRed, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelRed, 0);
        lv_obj_set_y(LabelRed, 70);
        lv_obj_set_align(LabelRed, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelRed, "Red");

        SliderRed = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderRed, 0, 255);
        lv_slider_set_value(SliderRed, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderRed) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderRed, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderRed, lv_pct(100));
        lv_obj_set_height(SliderRed, 20);
        lv_obj_set_x(SliderRed, 0);
        lv_obj_set_y(SliderRed, 90);
        lv_obj_set_align(SliderRed, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderRed, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderRed, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelGreen = lv_label_create(ledPanel);
        lv_obj_set_width(LabelGreen, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelGreen, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelGreen, 0);
        lv_obj_set_y(LabelGreen, 120);
        lv_obj_set_align(LabelGreen, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelGreen, "Green");

        SliderGreen = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderGreen, 0, 255);
        lv_slider_set_value(SliderGreen, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderGreen) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderGreen, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderGreen, lv_pct(100));
        lv_obj_set_height(SliderGreen, 20);
        lv_obj_set_x(SliderGreen, 0);
        lv_obj_set_y(SliderGreen, 140);
        lv_obj_set_align(SliderGreen, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderGreen, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderGreen, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelBlue = lv_label_create(ledPanel);
        lv_obj_set_width(LabelBlue, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelBlue, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelBlue, 0);
        lv_obj_set_y(LabelBlue, 170);
        lv_obj_set_align(LabelBlue, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelBlue, "Blue");

        SliderBlue = lv_slider_create(ledPanel);
        lv_slider_set_range(SliderBlue, 0, 255);
        lv_slider_set_value(SliderBlue, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderBlue) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderBlue, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderBlue, lv_pct(100));
        lv_obj_set_height(SliderBlue, 20);
        lv_obj_set_x(SliderBlue, 0);
        lv_obj_set_y(SliderBlue, 190);
        lv_obj_set_align(SliderBlue, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderBlue, lv_color_hex(0x0000FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderBlue, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(SliderRed, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderGreen, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderBlue, onColorSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(LedOnCheckBox, ledCheckBox_event_handler, LV_EVENT_ALL, NULL);
    }

    void initUI(lv_obj_t *container)
    {
        ledPanel = lv_obj_create(container);
        lv_obj_set_height(ledPanel, lv_pct(100));
        lv_obj_set_width(ledPanel, lv_pct(100));
        lv_obj_set_flex_flow(ledPanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(ledPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(ledPanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(ledPanel, false);

        initLedPanel();
    }

}