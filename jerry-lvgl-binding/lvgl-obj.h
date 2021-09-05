#ifndef LVGL_OBJ_H
#define LVGL_OBJ_H

void js_lvgl_obj_init();

jerry_value_t lvgl_obj_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc);
const char *get_lvgl_obj_constructor_name();

#endif /* LVGL_OBJ_H */