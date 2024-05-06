#include "uc2ui_custompage.h"
#include "lvgl_helper.h"

namespace uc2ui_custompage
{
    bool custom_module = false;
    int custom_count;
    bool custom_on = false;

    lv_obj_t *customPanel;
    lv_obj_t *ButtonCustom[6]; // Array to hold the custom buttons

    void (*updatedButtonListener)(char *topic, int value);

    void setCustomValueChangedListener(void valueChangedListener(char *topic, int value))
    {
        updatedButtonListener = valueChangedListener;
    }

    void setCustomModule(bool enable)
    {
        custom_module = enable;
        lvgl_helper::setVisibility(customPanel, enable);
    }

    void setCustomCount(int count)
    {
        custom_count = count;
    }

    void onCustomButtonClicked(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code == LV_EVENT_CLICKED)
        {
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            // Get the index of the button that was clicked

            int buttonIndex = 0;
            for (int i = 0; i < 6; ++i)
            {
                if (ButtonCustom[i] == obj)
                {
                    log_i("button target id %d, target %d", ButtonCustom[i], obj);
                    buttonIndex = i;
                    if (updatedButtonListener != nullptr)
                        updatedButtonListener("custom", buttonIndex);
                    break;
                }
            // Do something with the button index
        }
    }}

    void customCheckBox_event_handler(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            log_i("Custom CheckBox Clicked");
            /*
            int r = lv_slider_get_value(SliderCh1);
            int g = lv_slider_get_value(SliderCh2);
            int b = lv_slider_get_value(SliderCh3);
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            if (enableCustomListner != nullptr)
                enableCustomListner(state, r, g, b);
            */
        }
    }

    void initCustomPanel()
    {

        const int num_rows = 2;
        const int num_cols = 3;
        const int button_width = 100;
        const int button_height = 40;
        const int margin_x = 10;
        const int margin_y = 10;
        const int total_width = num_cols * (button_width + margin_x) - margin_x;
        const int total_height = num_rows * (button_height + margin_y) - margin_y;
        const int start_x = (lv_obj_get_width(customPanel) - total_width) / 2;
        const int start_y = (lv_obj_get_height(customPanel) - total_height) / 2;

        for (int row = 0; row < num_rows; ++row)
        {
            for (int col = 0; col < num_cols; ++col)
            {
                int index = row * num_cols + col;
                ButtonCustom[index] = lv_btn_create(customPanel);
                lv_obj_set_pos(ButtonCustom[index], start_x + col * (button_width + margin_x), start_y + row * (button_height + margin_y));
                lv_obj_set_size(ButtonCustom[index], button_width, button_height);
                lv_obj_t *label = lv_label_create(ButtonCustom[index]);
                char name[10];
                log_i("Custom %d, %d", row, col);
                lv_label_set_text(label, name);
                lv_obj_add_event_cb(ButtonCustom[index], onCustomButtonClicked, LV_EVENT_CLICKED, NULL);
            }
        }
    }

    void initUI(lv_obj_t *container)
    {
        customPanel = lv_obj_create(container);
        lv_obj_set_height(customPanel, lv_pct(100));
        lv_obj_set_width(customPanel, lv_pct(100));
        lv_obj_set_flex_flow(customPanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(customPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(customPanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(customPanel, true);

        initCustomPanel();
    }

}
