#include "lvgl_helper.h"

namespace lvgl_helper
{
    void setVisibility(lv_obj_t *ob, bool vis)
    {
        if (vis)
            lv_obj_clear_flag(ob, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(ob, LV_OBJ_FLAG_HIDDEN);
        lv_obj_update_layout(ob);
    }
}