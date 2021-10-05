---
to: ../jerry-lvgl-binding/<%= 'lvgl-' + h.changeCase.lower(widgetName) %>.c
---
#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"
#include "lvgl-obj.h"
#include "lvgl-<%= h.changeCase.lower(widgetName) %>.h"


static const char *NAME = "Lvgl<%= h.capitalize(widgetName) %>";

static void lvgl_<%= h.changeCase.lower(widgetName) %>_free_cb (void *native_p, jerry_object_native_info_t *info_p) {
    BI_LOG_TRACE("deconstruct <%= h.changeCase.lower(widgetName) %>\n");
    lv_obj_t *obj = (lv_obj_t *) native_p;
    js_lvgl_detach_children(obj);
    lv_obj_del(obj);
}

static const jerry_object_native_info_t lvgl_<%= h.changeCase.lower(widgetName) %>_native_info = {
    .free_cb = lvgl_<%= h.changeCase.lower(widgetName) %>_free_cb,
};

static jerry_value_t lvgl_<%= h.changeCase.lower(widgetName) %>_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_obj_t *obj = lv_<%= h.changeCase.lower(widgetName) %>_create(js_lvgl_get_detach_screen());
    jerry_set_object_native_pointer(
        info->this_value,
        obj,
        &lvgl_<%= h.changeCase.lower(widgetName) %>_native_info
    );
    return jerry_create_undefined();
}

static const jerry_function_entry js_<%= h.changeCase.lower(widgetName) %>_prototype_methods[] = {
};

void js_lvgl_<%= h.changeCase.lower(widgetName) %>_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t LvglObj = jerryx_get_property_str(global, get_lvgl_obj_constructor_name());
    if (jerry_value_is_undefined(LvglObj)) {
        return;
    }
    jerry_value_t obj_prototype = jerryx_get_property_str(LvglObj, "prototype");

    jerry_value_t constructor = jerry_create_external_function(lvgl_<%= h.changeCase.lower(widgetName) %>_constructor);
    jerry_value_t proto = jerry_create_object();
    jerry_set_prototype(proto, obj_prototype);
    jerry_set_prop_list(proto, js_<%= h.changeCase.lower(widgetName) %>_prototype_methods, countof(js_<%= h.changeCase.lower(widgetName) %>_prototype_methods));
    jerry_value_t native_info = jerry_create_number((uint32_t) &lvgl_<%= h.changeCase.lower(widgetName) %>_native_info);
    jerryx_set_property_str(proto, NATIVE_INFO_PROP_STR, native_info);
    jerryx_set_property_str(constructor, "prototype", proto);

    jerryx_set_property_str(global, NAME, constructor);

    jerry_release_value(global);
    jerry_release_value(LvglObj);
    jerry_release_value(obj_prototype);
    jerry_release_value(constructor);
    jerry_release_value(proto);
}
