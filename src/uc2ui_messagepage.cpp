#include "uc2ui_messagepage.h"
#include "lvgl_helper.h"

namespace uc2ui_messagepage
{
    bool message_module = false;
    int message_count;
    bool message_on = false;

    lv_obj_t *messagePanel;
    lv_obj_t *ButtonMessage[6]; // Array to hold the message buttons

    void (*updatedButtonListener)(char *topic, int value);

    void setMessageValueChangedListener(void valueChangedListener(char *topic, int value))
    {
        updatedButtonListener = valueChangedListener;

    }

    void setMessageModule(bool enable)
    {
        message_module = enable;
        lvgl_helper::setVisibility(messagePanel, enable);
    }

    void setMessageCount(int count)
    {
        message_count = count;
    }

    void onMessageButtonClicked(lv_event_t *e)
    {
                // {"task":"/message_act", "key":1, "value":0}
		/*
		data lookup:
		{
			key 	|	 meaning 		|	value
			--------------------------------
			1		|	snap image		|	0
			2		|	exposure time	|	0....1000000
			3		|	gain			|	0...100

		*/
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code == LV_EVENT_CLICKED)
        {
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            // Get the index of the button that was clicked

            int buttonIndex = 0;
            for (int i = 0; i < 6; ++i)
            {
                if (ButtonMessage[i] == obj)
                {
                    buttonIndex = i;
                    if (updatedButtonListener != nullptr){
                        updatedButtonListener("message", buttonIndex);
                        log_i("button target id %d, target %d", ButtonMessage[i], obj);
                    }
                    break;
                }
        }
    }}

    void messageCheckBox_event_handler(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            log_i("Message CheckBox Clicked");
            /*
            int r = lv_slider_get_value(SliderCh1);
            int g = lv_slider_get_value(SliderCh2);
            int b = lv_slider_get_value(SliderCh3);
            bool state = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            if (enableMessageListner != nullptr)
                enableMessageListner(state, r, g, b);
            */
        }
    }

    void initMessagePanel()
    {

        const int num_rows = 2;
        const int num_cols = 3;
        const int button_width = 100;
        const int button_height = 40;
        const int margin_x = 10;
        const int margin_y = 10;
        const int total_width = num_cols * (button_width + margin_x) - margin_x;
        const int total_height = num_rows * (button_height + margin_y) - margin_y;
        const int start_x = (lv_obj_get_width(messagePanel) - total_width) / 2;
        const int start_y = (lv_obj_get_height(messagePanel) - total_height) / 2;

        for (int row = 0; row < num_rows; ++row)
        {
            for (int col = 0; col < num_cols; ++col)
            {
                int index = row * num_cols + col;
                ButtonMessage[index] = lv_btn_create(messagePanel);
                lv_obj_set_pos(ButtonMessage[index], start_x + col * (button_width + margin_x), start_y + row * (button_height + margin_y));
                lv_obj_set_size(ButtonMessage[index], button_width, button_height);
                lv_obj_t *label = lv_label_create(ButtonMessage[index]);
                char name[10];
                log_i("Message %d, %d", row, col);
                lv_label_set_text(label, name);
                lv_obj_add_event_cb(ButtonMessage[index], onMessageButtonClicked, LV_EVENT_CLICKED, NULL);
            }
        }
    }

    void initUI(lv_obj_t *container)
    {
        messagePanel = lv_obj_create(container);
        lv_obj_set_height(messagePanel, lv_pct(100));
        lv_obj_set_width(messagePanel, lv_pct(100));
        lv_obj_set_flex_flow(messagePanel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(messagePanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(messagePanel, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_helper::setVisibility(messagePanel, true);

        initMessagePanel();
    }

}
