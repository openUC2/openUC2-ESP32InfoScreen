#include "uc2ui_controller.h"
#include "uc2ui_wifipage.h"
#include "Arduino.h"
#include "uc2ui_controlpage.h"

namespace uc2ui_controller
{
    lv_obj_t *mainScreen;
    lv_obj_t *keyboard;
    lv_obj_t *ui_MainTabView;

    lv_obj_t *wifiPage;
    lv_obj_t *controlPage;

    void on_textarea_focus_event(lv_event_t *e)
    {
        //log_i("on_textarea_focus_event");
        lv_event_code_t event_code = lv_event_get_code(e);
        lv_obj_t *target = lv_event_get_target(e);
        if (event_code == LV_EVENT_FOCUSED)
        {
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_textarea(keyboard, target);
            lv_obj_update_layout(mainScreen);
            lv_obj_scroll_to_y(mainScreen, LV_COORD_MAX , LV_ANIM_ON);
        }
        if (event_code == LV_EVENT_DEFOCUSED)
        {
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_obj_update_layout(mainScreen);
            lv_obj_scroll_to_y(mainScreen, LV_COORD_MAX , LV_ANIM_ON);
        }
    }

    void initUi()
    {
        lv_disp_t *display = lv_disp_get_default();
        lv_theme_t *theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                                  true, LV_FONT_DEFAULT);
        lv_disp_set_theme(display, theme);
        // create mainscreen
        mainScreen = lv_obj_create(NULL);
        //lv_obj_clear_flag(mainScreen, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_flex_flow(mainScreen, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(mainScreen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

        ui_MainTabView = lv_tabview_create(mainScreen, LV_DIR_TOP, 50);
        lv_obj_set_width(ui_MainTabView, lv_pct(100));
        lv_obj_set_height(ui_MainTabView, lv_pct(100));
        lv_obj_set_align(ui_MainTabView, LV_ALIGN_CENTER);
        lv_obj_clear_flag(ui_MainTabView, LV_OBJ_FLAG_SCROLLABLE); /// Flags

        keyboard = lv_keyboard_create(mainScreen);
        lv_obj_set_width(keyboard, 800);
        lv_obj_set_height(keyboard, 240);
        lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        wifiPage = lv_tabview_add_tab(ui_MainTabView, "Wifi");
        uc2ui_wifipage::init_ui(wifiPage, on_textarea_focus_event);

        controlPage = lv_tabview_add_tab(ui_MainTabView, "Control");
        uc2ui_controlpage::uiInit(controlPage,on_textarea_focus_event);

        lv_disp_load_scr(mainScreen);
    }

}