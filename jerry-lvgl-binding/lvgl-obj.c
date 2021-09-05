#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

static const char *NAME = "LvglObj";

static void lvgl_obj_free_cb (void *native_p, jerry_object_native_info_t *info_p)
{
    BI_LOG_TRACE("deconstruct\n");
    lv_obj_t *obj = (lv_obj_t *) native_p;
    lv_obj_del(obj);
}

static const jerry_object_native_info_t lvgl_obj_native_info = {
    .free_cb = lvgl_obj_free_cb,
};

#include "obj-style-gen.h"

jerry_value_t lvgl_obj_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    jerry_set_object_native_pointer(
        info->this_value,
        obj,
        &lvgl_obj_native_info
    );
    return jerry_create_undefined();
}

/**
 * free_cb在gc的时候有没有调用
 */
void js_lvgl_obj_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t constructor = jerry_create_external_function(lvgl_obj_constructor);
    jerry_value_t proto = jerry_create_object();
    jerry_set_prop_list(proto, js_obj_prototype_methods, countof(js_obj_prototype_methods));
    jerryx_set_property_str(constructor, "prototype", proto);

    jerryx_set_property_str(global, NAME, constructor);

    jerry_release_value(global);
    jerry_release_value(constructor);
    jerry_release_value(proto);
}

const char *get_lvgl_obj_constructor_name() {
    return NAME;
}

const jerry_object_native_info_t *get_lvgl_obj_native_info() {
    return &lvgl_obj_native_info;
}
