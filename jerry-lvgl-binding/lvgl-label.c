#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"
#include "lvgl-obj.h"
#include "lvgl-label.h"


static const char *NAME = "LvglLabel";

static void lvgl_label_free_cb(void *native_p, jerry_object_native_info_t *info_p) {
    BI_LOG_TRACE("deconstruct label");
    lv_obj_t *obj = (lv_obj_t *) native_p;
    lvgl_obj_desctruct(obj);
}

static const jerry_object_native_info_t lvgl_label_native_info = {
    .free_cb = lvgl_label_free_cb,
};

static jerry_value_t lvgl_label_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_obj_t *obj = lv_label_create(js_lvgl_get_detach_screen());
    jerry_set_object_native_pointer(
        info->this_value,
        obj,
        &lvgl_label_native_info
    );
    return jerry_create_undefined();
}

static jerry_value_t js_lvgl_label_set_text(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    BI_LOG_TRACE("lebel set text");
    void *obj = NULL;
    if (
        argc != 0 
        && jerry_value_is_string(argv[0])
        && js_lvgl_get_native_info(info->this_value, &obj)
    ) {
        char *text = jerry_to_c_string(argv[0]);
        lv_label_set_text(obj, text);
        jerry_free_c_string(text);
        BI_LOG_TRACE("lebel set text complete");
    }

    return jerry_create_undefined();
}

static const jerry_function_entry js_label_prototype_methods[] = {
    JERRY_CFUNC_DEF("setText", js_lvgl_label_set_text),
};

void js_lvgl_label_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t LvglObj = jerryx_get_property_str(global, get_lvgl_obj_constructor_name());
    if (jerry_value_is_undefined(LvglObj)) {
        return;
    }
    jerry_value_t obj_prototype = jerryx_get_property_str(LvglObj, "prototype");
 
    jerry_value_t constructor = jerry_create_external_function(lvgl_label_constructor);
    jerry_value_t proto = jerry_create_object();
    jerry_set_prototype(proto, obj_prototype);
    jerry_set_prop_list(proto, js_label_prototype_methods, countof(js_label_prototype_methods));
    jerry_value_t native_info = jerry_create_number((uint32_t) &lvgl_label_native_info);
    jerryx_set_property_str(proto, NATIVE_INFO_PROP_STR, native_info);
    jerryx_set_property_str(constructor, "prototype", proto);

    jerryx_set_property_str(global, NAME, constructor);

    jerry_release_value(global);
    jerry_release_value(LvglObj);
    jerry_release_value(obj_prototype);
    jerry_release_value(constructor);
    jerry_release_value(proto);
}
