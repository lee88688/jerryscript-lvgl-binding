#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

#define STYLE_ITEM_LAYOUT_FLEX "s(LV_LAYOUT_FLEX)"

#define STYLE_ITEM_FLEX_FLOW "s(FLEX_FLOW)"
#define STYLE_ITEM_FLEX_MAIN_PLACE "s(FLEX_MAIN_PLACE)"
#define STYLE_ITEM_FLEX_CROSS_PLACE "s(FLEX_CROSS_PLACE)"
#define STYLE_ITEM_FLEX_TRACK_PLACE "s(FLEX_TRACK_PLACE)"

typedef struct _lvgl_style_value_map {
    char *name;
    void *value;
} lvgl_style_value_map_t;


const lvgl_style_value_map_t LVGL_STYLE_PROP_MAP[] = {
    { .name=STYLE_ITEM_FLEX_FLOW, .value=&LV_STYLE_FLEX_FLOW },
    { .name=STYLE_ITEM_FLEX_MAIN_PLACE, .value=&LV_STYLE_FLEX_MAIN_PLACE },
    { .name=STYLE_ITEM_FLEX_CROSS_PLACE, .value=&LV_STYLE_FLEX_CROSS_PLACE },
    { .name=STYLE_ITEM_FLEX_TRACK_PLACE, .value=&LV_STYLE_FLEX_TRACK_PLACE },
};

static void lvgl_style_free_cb (void *native_p, jerry_object_native_info_t *info_p) {
    BI_LOG_TRACE("free style");
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
    // style->is_const = 1; // prevent lvgl modifying
    style->has_group = 0xff; // LV_STYLE_CONST_INIT
    jerry_value_t attributes = jerryx_get_property_str(obj_prop_value, "attributes");
    uint8_t prop_cnt = jerry_get_array_length(attributes);
    if (prop_cnt > 1) {
        style->prop_cnt = prop_cnt;
        // lv_style_const_prop_t *style_props = malloc(sizeof(lv_style_const_prop_t) * prop_cnt);
        uint8_t *values_and_props = (uint8_t *) malloc(style->prop_cnt * (sizeof(lv_style_value_t) + sizeof(uint16_t)));
        uint16_t *props = (uint16_t *) (values_and_props + style->prop_cnt * sizeof(lv_style_value_t));
        lv_style_value_t *values = (lv_style_value_t *) values_and_props;
        uint8_t i = 0;
        for (i = 0; i < prop_cnt; i++) {
            jerry_value_t attr = jerry_get_property_by_index(attributes, i);
            jerry_value_t name = jerryx_get_property_str(attr, "name");
            jerry_value_t value = jerryx_get_property_str(attr, "value");

            lv_style_prop_t prop_name;
            if (jerry_value_is_string(name)) {
                char *s_name = jerry_to_c_string(name);
                uint32_t i = 0;
                bool found = false;
                for (i = 0; i < countof(LVGL_STYLE_PROP_MAP); i++) {
                    if (strcmp(LVGL_STYLE_PROP_MAP[i].name, s_name) == 0) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    lv_style_prop_t *prop = (lv_style_prop_t *) LVGL_STYLE_PROP_MAP[i].value;
                    prop_name = *prop;
                } else {
                    BI_LOG_ERROR("style item prop(%s) is not found in lvgl prop", s_name);
                    prop_name = LV_STYLE_PROP_INV; // give a default value. this should not be happen.
                }
                jerry_free_c_string(s_name);
            } else {
                prop_name = jerry_value_as_uint32(name);
            }
            props[i] = prop_name;
            // style_props[i].prop = prop_name;
            switch (prop_name) {
                // color
                case LV_STYLE_BORDER_COLOR:
                case LV_STYLE_LINE_COLOR:
                case LV_STYLE_TEXT_COLOR:
                case LV_STYLE_OUTLINE_COLOR:
                case LV_STYLE_SHADOW_COLOR:
                case LV_STYLE_ARC_COLOR:
                case LV_STYLE_BG_GRAD_COLOR:
                case LV_STYLE_BG_IMG_RECOLOR:
                case LV_STYLE_IMG_RECOLOR:
                case LV_STYLE_BG_COLOR: {
                    uint32_t v = jerry_value_as_uint32(value);
                    lv_style_value_t style_value = { .color = LV_COLOR_MAKE((v >> 16) & 0xff, (v >> 8) & 0xff, v & 0xff) };
                    // lv_style_value_t style_value;
                    // lv_color_t color = LV_COLOR_MAKE(((v >> 16) & 0xff), ((v >> 8) & 0xff), (v & 0xff));
                    // style_value.color = color;
                    values[i] = style_value;
                    // style_props[i].value = style_value;
                    break;
                }

                // currently only support flex layout
                case LV_STYLE_LAYOUT: {
                    values[i].num = LV_LAYOUT_FLEX;
                    // style_props[i].value.num = LV_LAYOUT_FLEX;
                    break;
                }
                
                // default for most of props
                default:
                    values[i].num = jerry_value_as_uint32(value);
                    // style_props[i].value.num = jerry_value_as_uint32(value);
                    break;
            }

            jerry_release_value(value);
            jerry_release_value(name);
            jerry_release_value(attr);
        }
        style->v_p.values_and_props = values_and_props;
        // style->v_p.const_props = style_props;
    } else if (prop_cnt == 1) {
        style->prop_cnt = prop_cnt;
        jerry_value_t attr = jerry_get_property_by_index(attributes, 0);
        jerry_value_t name = jerryx_get_property_str(attr, "name");
        jerry_value_t value = jerryx_get_property_str(attr, "value");

        // todo: set prop value need int32
        style->prop1 = jerry_value_as_uint32(name);
        style->v_p.value1.num = jerry_value_as_int32(value);
        // lv_style_set_prop(style, jerry_value_as_uint32(name), (lv_style_value_t) {.num = jerry_value_as_int32(value)});

        jerry_release_value(attr);
        jerry_release_value(name);
        jerry_release_value(value);
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
    if (iteration_result) {
        return jerry_acquire_value(argv[0]);
    }

    return jerry_create_undefined();
}

void js_lvgl_style_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t func = jerry_create_external_function(create_lvgl_style);
    jerryx_set_property_str(global, "createLvglStyles", func);

    jerry_release_value(global);
}

jerry_object_native_info_t *get_lvgl_style_native_info() {
    return &lvgl_style_native_info;
}