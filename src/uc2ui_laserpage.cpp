#include "uc2ui_laserpage.h"
#include "lvgl_helper.h"

namespace uc2ui_laserpage
{
    bool laser_module = false;
    int laser_count;
    bool laser_on = false;

    lv_obj_t *laserPanel;
    lv_obj_t *LaserOnCheckBox;
    lv_obj_t *LabelBlue;
    lv_obj_t *LabelGreen;
    lv_obj_t *LabelRed;
    lv_obj_t *SliderCh3;
    lv_obj_t *SliderCh2;
    lv_obj_t *SliderCh1;

    void (*updatedValuesListener)(int r, int g, int b);

    void setLaserValueChangedListener(void valueChangedListener(int r, int g, int b))
    {
        updatedValuesListener = valueChangedListener;
    }

    void (*enableLaserListner)(bool enable, int r, int g, int b);

    void setenableLaserListner(void laserlistner(bool enable, int r, int g, int b))
    {
        enableLaserListner = laserlistner;
    }

    void setLaserModule(bool enable)
    {
        laser_module = enable;
        lvgl_helper::setVisibility(laserPanel, enable);
    }

    void setLaserOn(bool on)
    {
        laser_on = on;
        if (on)
            lv_obj_add_state(LaserOnCheckBox, LV_STATE_CHECKED);
        else
            lv_obj_clear_state(LaserOnCheckBox, LV_STATE_CHECKED);
    }

    void setLaserCount(int count)
    {
        laser_count = count;
    }

    void onValueSliderChanged(lv_event_t *e)
    {
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderCh1);
            int g = lv_slider_get_value(SliderCh2);
            int b = lv_slider_get_value(SliderCh3);
            if (updatedValuesListener != nullptr)
                updatedValuesListener(r, g, b);
        }
    }

    void laserCheckBox_event_handler(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int r = lv_slider_get_value(SliderCh1);
            int g = lv_slider_get_value(SliderCh2);
            int b = lv_slider_get_value(SliderCh3);
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            if (enableLaserListner != nullptr)
                enableLaserListner(state, r, g, b);
        }
    }

    void initLaserPanel()
    {

        LaserOnCheckBox = lv_checkbox_create(laserPanel);
        lv_checkbox_set_text(LaserOnCheckBox, "Laser On");
        lv_obj_set_width(LaserOnCheckBox, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LaserOnCheckBox, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LaserOnCheckBox, -130);
        lv_obj_set_y(LaserOnCheckBox, 30);
        lv_obj_set_align(LaserOnCheckBox, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(LaserOnCheckBox, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags

        LabelRed = lv_label_create(laserPanel);
        lv_obj_set_width(LabelRed, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelRed, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelRed, 0);
        lv_obj_set_y(LabelRed, 70);
        lv_obj_set_align(LabelRed, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelRed, "Channel 1");

        SliderCh1 = lv_slider_create(laserPanel);
        lv_slider_set_range(SliderCh1, 0, 255);
        lv_slider_set_value(SliderCh1, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderCh1) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderCh1, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderCh1, lv_pct(100));
        lv_obj_set_height(SliderCh1, 20);
        lv_obj_set_x(SliderCh1, 0);
        lv_obj_set_y(SliderCh1, 90);
        lv_obj_set_align(SliderCh1, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderCh1, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderCh1, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelGreen = lv_label_create(laserPanel);
        lv_obj_set_width(LabelGreen, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelGreen, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelGreen, 0);
        lv_obj_set_y(LabelGreen, 120);
        lv_obj_set_align(LabelGreen, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelGreen, "Channel 2");

        SliderCh2 = lv_slider_create(laserPanel);
        lv_slider_set_range(SliderCh2, 0, 255);
        lv_slider_set_value(SliderCh2, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderCh2) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderCh2, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderCh2, lv_pct(100));
        lv_obj_set_height(SliderCh2, 20);
        lv_obj_set_x(SliderCh2, 0);
        lv_obj_set_y(SliderCh2, 140);
        lv_obj_set_align(SliderCh2, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderCh2, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderCh2, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh2, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        LabelBlue = lv_label_create(laserPanel);
        lv_obj_set_width(LabelBlue, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(LabelBlue, LV_SIZE_CONTENT); /// 1
        lv_obj_set_x(LabelBlue, 0);
        lv_obj_set_y(LabelBlue, 170);
        lv_obj_set_align(LabelBlue, LV_ALIGN_TOP_MID);
        lv_label_set_text(LabelBlue, "Channel 3");

        SliderCh3 = lv_slider_create(laserPanel);
        lv_slider_set_range(SliderCh3, 0, 255);
        lv_slider_set_value(SliderCh3, 255, LV_ANIM_OFF);
        if (lv_slider_get_mode(SliderCh3) == LV_SLIDER_MODE_RANGE)
            lv_slider_set_left_value(SliderCh3, 0, LV_ANIM_OFF);
        lv_obj_set_width(SliderCh3, lv_pct(100));
        lv_obj_set_height(SliderCh3, 20);
        lv_obj_set_x(SliderCh3, 0);
        lv_obj_set_y(SliderCh3, 190);
        lv_obj_set_align(SliderCh3, LV_ALIGN_TOP_MID);
        lv_obj_set_style_bg_color(SliderCh3, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(SliderCh3, lv_color_hex(0x0000FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(SliderCh3, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

        lv_obj_add_event_cb(SliderCh1, onValueSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderCh2, onValueSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(SliderCh3, onValueSliderChanged, LV_EVENT_ALL, NULL);
        lv_obj_add_event_cb(LaserOnCheckBox, laserCheckBox_event_handler, LV_EVENT_ALL, NULL);
    }

    void initUI(lv_obj_t *container)
    {
        laserPanel = lv_obj_create(container);
        lv_obj_set_height(laserPanel, lv_pct(100));
        lv_obj_set_width(laserPanel, lv_pct(100));
        lv_obj_set_flex_flow(laserPanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(laserPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(laserPanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(laserPanel, true);

        initLaserPanel();
    }

}