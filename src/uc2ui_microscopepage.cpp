#include "uc2ui_microscopepage.h"
#include "uc2ui_motorpage.h"
#include "uc2ui_ledpage.h"

namespace uc2ui_microscopepage
{
    lv_obj_t * tabViewMicroscopeControl;
    lv_obj_t * motorpage;
    lv_obj_t * ledpage;
    void initUi(lv_obj_t *motoradvpage)
    {
        tabViewMicroscopeControl = lv_tabview_create(motoradvpage, LV_DIR_TOP, 40);
        lv_obj_set_width(tabViewMicroscopeControl, lv_pct(100));
        lv_obj_set_height(tabViewMicroscopeControl, lv_pct(100));
        lv_obj_set_x(tabViewMicroscopeControl, 0);
        lv_obj_set_y(tabViewMicroscopeControl, -17);
        lv_obj_set_align(tabViewMicroscopeControl, LV_ALIGN_CENTER);
        lv_obj_clear_flag(lv_tabview_get_content(tabViewMicroscopeControl), LV_OBJ_FLAG_SCROLLABLE);

        motorpage = lv_tabview_add_tab(tabViewMicroscopeControl, "Motor");
        lv_obj_clear_flag(motorpage, LV_OBJ_FLAG_SCROLLABLE);
        uc2ui_motorpage::initUI(motorpage);

        ledpage = lv_tabview_add_tab(tabViewMicroscopeControl, "Led");
        uc2ui_ledpage::initUI(ledpage);
    }
}
// namespace uc2ui_motoradvpage