#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"


static void lvgl_style_free_cb (void *native_p, jerry_object_native_info_t *info_p) {
    lv_style_t *style = (lv_obj_t *) native_p;
    // todo: prop_cnt == 1
    if (style->prop_cnt > 0) {
        free((void *) style->v_p.const_props);
    }
    free(style);
}

static const jerry_object_native_info_t lvgl_style_native_info = {
    .free_cb = lvgl_style_free_cb,
};

static bool create_lvgl_style_foreach(const jerry_value_t obj_prop_name, const jerry_value_t obj_prop_value, void* user_data) {
    jerry_value_t* argv = (jerry_value_t *) user_data;
    jerry_value_t obj = argv[0];

    lv_style_t *style = (lv_style_t *) malloc(sizeof(lv_style_t));
    lv_style_init(style);
    style->is_const = 1; // prevent lvgl modifying
    jerry_value_t attributes = jerryx_get_property_str(obj_prop_value, "attributes");
    uint8_t prop_cnt = jerry_get_array_length(attributes);
    // todo: prop_cnt == 1
    if (prop_cnt != 0) {
        style->prop_cnt = prop_cnt;
        lv_style_const_prop_t *style_props = malloc(sizeof(lv_style_const_prop_t) * prop_cnt);
        uint8_t i = 0;
        for (i = 0; i < prop_cnt; i++) {
            jerry_value_t attr = jerry_get_property_by_index(attributes, i);
            jerry_value_t name = jerryx_get_property_str(attr, "name");
            jerry_value_t value = jerryx_get_property_str(attr, "value");

            lv_style_prop_t prop_name = jerry_value_as_uint32(name);
            style_props[i].prop = prop_name;
            switch (prop_name) {
                // color
                case LV_STYLE_TEXT_COLOR:
                case LV_STYLE_BG_COLOR: {
                    uint32_t v = jerry_value_as_uint32(value);
                    lv_style_value_t style_value = { .color = LV_COLOR_MAKE((v >> 16) & 0xff, (v >> 8) & 0xff, v & 0xff) };
                    // lv_style_value_t style_value;
                    // lv_color_t color = LV_COLOR_MAKE(((v >> 16) & 0xff), ((v >> 8) & 0xff), (v & 0xff));
                    // style_value.color = color;
                    style_props[i].value = style_value;
                    break;
                }
                
                // default for most of props
                default:
                    style_props[i].value.num = jerry_value_as_uint32(value);
                    break;
            }

            jerry_release_value(value);
            jerry_release_value(name);
            jerry_release_value(attr);
        }
        style->v_p.const_props = style_props;
    }

    jerry_set_object_native_pointer(
        obj_prop_value,
        style,
        &lvgl_style_native_info
    );

    jerry_release_value(attributes);
    return true;
}

static jerry_value_t create_lvgl_style(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    // jerry_value_t js_style_obj = argv[0];
    bool iteration_result = jerry_foreach_object_property(argv[0], create_lvgl_style_foreach, (void *) argv);

    return jerry_create_undefined();
}

void js_lvgl_style_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t func = jerry_create_external_function(create_lvgl_style);
    jerryx_set_property_str(global, "createLvglStyles", func);

    jerry_release_value(global);
}