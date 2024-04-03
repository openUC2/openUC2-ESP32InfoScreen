#pragma once
#include "lvgl.h"
#include "esp_heap_caps.h"
#include "ESP_Panel_Library.h"

namespace lvgl_controller
{

/* LVGL porting configurations */
#define LVGL_TICK_PERIOD_MS (2)
#define LVGL_TASK_MAX_DELAY_MS (500)
#define LVGL_TASK_MIN_DELAY_MS (1)
#define LVGL_TASK_STACK_SIZE (8 * 1024)
#define LVGL_TASK_PRIORITY (2)
#define LVGL_BUF_SIZE (ESP_PANEL_LCD_H_RES * 20)

    void initlgvl();
};