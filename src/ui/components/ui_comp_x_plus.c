// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "../ui.h"


// COMPONENT X Plus

lv_obj_t *ui_X_Plus_create(lv_obj_t *comp_parent) {

lv_obj_t *cui_X_Plus;
cui_X_Plus = lv_btn_create(comp_parent);
lv_obj_set_width( cui_X_Plus, 135);
lv_obj_set_height( cui_X_Plus, 50);
lv_obj_set_x( cui_X_Plus, -252 );
lv_obj_set_y( cui_X_Plus, -105 );
lv_obj_set_align( cui_X_Plus, LV_ALIGN_CENTER );
lv_obj_add_flag( cui_X_Plus, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( cui_X_Plus, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_text_color(cui_X_Plus, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_X_Plus, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_X_PLUS_NUM);
children[UI_COMP_X_PLUS_X_PLUS] = cui_X_Plus;
lv_obj_add_event_cb(cui_X_Plus, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
lv_obj_add_event_cb(cui_X_Plus, del_component_child_event_cb, LV_EVENT_DELETE, children);
ui_comp_X_Plus_create_hook(cui_X_Plus);
return cui_X_Plus; 
}
