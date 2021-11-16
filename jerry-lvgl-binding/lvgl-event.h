#ifndef LVGL_EVENT_H
#define LVGL_EVENT_H

#include <stdint.h>
#include "jerryscript.h"
#include "lvgl.h"

typedef struct _lvgl_event_user_data {
    jerry_value_t target;
    jerry_value_t func;
} lvgl_event_user_data_t;


jerry_value_t lvgl_event_create(lv_event_t *event);

void lvgl_event_clear_after_call(jerry_value_t e);

lvgl_event_user_data_t *lvgl_event_get_user_data_from_obj(lv_obj_t *obj, jerry_value_t func);

void lvgl_event_clear_user_data(lv_obj_t *obj);

#endif /* LVGL_EVENT_H */