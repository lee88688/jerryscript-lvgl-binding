#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

void _bi_log(lv_log_level_t level, const char * file, int line, const char * func, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    char msg[256];
    lv_vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    /*Use only the file name not the path*/
    size_t p;
    for(p = strlen(file); p > 0; p--) {
        if(file[p] == '/' || file[p] == '\\') {
            p++;    /*Skip the slash*/
            break;
        }
    }

    char buf[512];
    uint32_t t = lv_tick_get();
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    lv_snprintf(buf, sizeof(buf), "[%s]\t%s: %s \t(in %s line #%d)\n", lvl_prefix[level], func, msg, &file[p], line);
    printf("%s", buf);
}

void jerry_set_prop_list(jerry_value_t value, const jerry_function_entry *entries, jerry_size_t len) {
    jerry_value_t result = jerry_create_undefined();
    for (jerry_size_t i = 0; i < len; i++) {
        switch (entries->def_type)
        {
        case JERRY_DEF_CFUNC: {
            jerry_value_t func = jerry_create_external_function(entries[i].u.func);
            result = jerryx_set_property_str(value, entries[i].name, func);
            jerry_release_value(func);
            break;
        }

        default:
            break;
        }
        JERRY_ERROR_HANDLE(result, "set prop list with errors.");
        jerry_release_value(result);
    }
}

char *jerry_to_c_string(jerry_value_t value) {
    if (jerry_value_is_string(value)) {
        jerry_length_t len = jerry_get_utf8_string_size(value);
        char *str = malloc(len + 1);
        jerry_string_to_utf8_char_buffer(value, (const jerry_char_t *)str, len + 1);
        str[len] = '\0';
        return str;
    }
    return NULL;
}

void jerry_free_c_string(char *str) {
    if (!str) {
        free(str);
    }
}

jerry_char_t *jerry_to_utf8_string(jerry_value_t value, jerry_size_t *len) {
    if (jerry_value_is_string(value) && len) {
        jerry_size_t size = jerry_get_utf8_string_size(value);
        jerry_char_t *str = malloc(size);
        *len = jerry_string_to_utf8_char_buffer(value, str, size);
        return str;
    }
    return NULL;
}

bool js_lvgl_get_native_info(jerry_value_t value, void **obj) {
    jerry_value_t native_value_p = jerryx_get_property_str(value, NATIVE_INFO_PROP_STR);
    if (jerry_value_is_undefined(native_value_p) && jerry_value_is_error(native_value_p)) {
        return false;
    }
    jerry_object_native_info_t *native_p = (jerry_object_native_info_t *) jerry_value_as_uint32(native_value_p);
    bool ret = jerry_get_object_native_pointer(value, obj, native_p);

    jerry_release_value(native_value_p);
    return ret;
}

lv_obj_t *js_lvgl_get_detach_screen() {
    static lv_obj_t *detach_screen = NULL;
    if (detach_screen == NULL) {
        detach_screen = lv_obj_create(NULL);
    }

    return detach_screen;
}

void js_lvgl_detach_children(lv_obj_t *parent) {
    uint32_t len = lv_obj_get_child_cnt(parent);

    while (len) {
        lv_obj_t *child = lv_obj_get_child(parent, len - 1);
        lv_obj_set_parent(child, js_lvgl_get_detach_screen()); // todo: change to detach screen
    }
}

void print_obj_style_info(lv_obj_t *obj, int32_t expand_index) {
    printf("[style info] obj: %d, style cnt: %d\n", obj, obj->style_cnt);
    uint32_t i = 0;
    for (i = 0; i < obj->style_cnt; i++) {
        printf("index: %d, is_local: %d, is_trans: %d, selector: %d\n", i, obj->styles[i].is_local, obj->styles[i].is_trans, obj->styles[i].selector);
        if (i == expand_index || expand_index == -1) {
            uint32_t j = 0;
            lv_style_t *style = obj->styles[i].style;
            printf("  [expand] prop_cnt: %d, is_const: %d\n", style->prop_cnt, style->is_const);
            if (style->prop_cnt > 1 && style->is_const) {
                for(j = 0; j < style->prop_cnt; j++) {
                    printf("    prop: %u, value: %d\n", style->v_p.const_props[j].prop, style->v_p.const_props[j].value.num);
                }
            } else if (style->prop_cnt > 1 && !style->is_const) {
                uint16_t *props = (uint16_t *) (style->v_p.values_and_props + style->prop_cnt * sizeof(lv_style_value_t));
                lv_style_value_t *values = (lv_style_value_t *) style->v_p.values_and_props;
                for(j = 0; j < style->prop_cnt; j++) {
                    printf("    prop: %u, value: %u\n", props[j], values[j].num);
                }
            } else {
                printf("    prop: %u, value: %u\n", style->prop1, style->v_p.value1.num);
            }
        }
    }
}


jerry_value_t js_array_shift(jerry_value_t arr) {
    jerry_value_t js_shift = jerryx_get_property_str(arr, "shift");
    jerry_value_t res = jerry_call_function(js_shift, arr, NULL, 0);

    jerry_release_value(js_shift);
    return res;
}

jerry_value_t js_array_push(jerry_value_t arr, jerry_value_t val) {
    jerry_value_t js_push = jerryx_get_property_str(arr, "push");
    jerry_value_t args[] = { val };
    jerry_value_t res = jerry_call_function(js_push, arr, args, 1);

    jerry_release_value(js_push);
    return res;
}

// inspect tool
void get_screen_obj_info() {
    lv_obj_t *screen = js_lvgl_get_detach_screen();
    jerry_value_t root = jerry_create_object();

    jerry_value_t id = jerry_create_number((uint32_t) screen);
    jerryx_set_property_str(root, "id", id);
    jerry_release_value(id);
    jerry_value_t queue = jerry_create_array(0);
    js_array_push(queue, root);

    while(jerry_get_array_length(queue) != 0) {
        jerry_value_t cur = js_array_shift(queue);
        jerry_value_t cur_id = jerryx_get_property_str(cur, "id");
        lv_obj_t *obj = (lv_obj_t *) jerry_value_as_uint32(cur_id);
        uint32_t i = 0;
        // add children to cur
        jerry_value_t js_children = jerry_create_array(0);
        for (i = 0; i < obj->spec_attr->child_cnt; i++) {
            jerry_value_t lvObj = jerry_create_object();
            jerry_value_t id = jerry_create_number((uint32_t) obj->spec_attr->children[i]);
            jerryx_set_property_str(lvObj, "id", id);
            js_array_push(queue, lvObj);
            js_array_push(js_children, lvObj);

            jerry_release_value(id);
            jerry_release_value(lvObj);
        }
        jerryx_set_property_str(cur, "children", js_children);

        // add styles to cur
        jerry_value_t styles = jerry_create_array(0);
        jerryx_set_property_str(cur, "styles", styles);
        for (i = 0; i < obj->style_cnt; i++) {
            jerry_value_t js_style = jerry_create_object();
            js_array_push(styles, js_style);

            lv_style_t *style = obj->styles[i].style;
            jerry_value_t js_id = jerry_create_number((uint32_t) style);
            jerryx_set_property_str(js_style, "id", js_id);
            jerryx_set_property_str(js_style, "isLocal", jerry_create_boolean(obj->styles[i].is_local));
            jerryx_set_property_str(js_style, "isTrans", jerry_create_boolean(obj->styles[i].is_trans));
            jerry_value_t js_selector = jerry_create_number(obj->styles[i].selector);
            jerryx_set_property_str(js_style, "selector", js_selector);
            jerryx_set_property_str(js_style, "isConst", jerry_create_boolean(style->is_const));
            jerry_value_t js_has_group = jerry_create_number(style->has_group);
            jerryx_set_property_str(js_style, "hasGroup", js_has_group);

            jerry_value_t js_attrs = jerry_create_array(0);
            if (style->prop_cnt == 1) {
                jerry_value_t js_prop_val = jerry_create_object();
                jerry_value_t js_name = jerry_create_number(style->prop1);
                jerryx_set_property_str(js_prop_val, "name", js_name);
                jerry_value_t js_value = jerry_create_number((uint32_t) style->v_p.value1.num);
                jerryx_set_property_str(js_prop_val, "value", js_value);
                js_array_push(js_attrs, js_prop_val);

                jerry_release_value(js_value);
                jerry_release_value(js_name);
                jerry_release_value(js_prop_val);
            } else if (style->is_const && style->prop_cnt > 1) {
                uint32_t j = 0;
                for(j = 0; j < style->prop_cnt; j++) {
                    jerry_value_t js_prop_val = jerry_create_object();
                    jerry_value_t js_name = jerry_create_number(style->v_p.const_props[j].prop);
                    jerryx_set_property_str(js_prop_val, "name", js_name);
                    jerry_value_t js_value = jerry_create_number((uint32_t) style->v_p.const_props[j].value.num);
                    jerryx_set_property_str(js_prop_val, "value", js_value);
                    js_array_push(js_attrs, js_prop_val);

                    jerry_release_value(js_value);
                    jerry_release_value(js_name);
                    jerry_release_value(js_prop_val);
                }
            } else if (!style->is_const && style->prop_cnt > 1) {
                uint32_t j = 0;
                for (j = 0; j < style->prop_cnt; j++) {
                    uint16_t *props = (uint16_t *) (style->v_p.values_and_props + style->prop_cnt * sizeof(lv_style_value_t));
                    lv_style_value_t *values = (lv_style_value_t *) style->v_p.values_and_props;

                    jerry_value_t js_prop_val = jerry_create_object();
                    jerry_value_t js_name = jerry_create_number(props[j]);
                    jerryx_set_property_str(js_prop_val, "name", js_name);
                    jerry_value_t js_value = jerry_create_number((uint32_t) values[j].num);
                    jerryx_set_property_str(js_prop_val, "value", js_value);
                    js_array_push(js_attrs, js_prop_val);

                    jerry_release_value(js_value);
                    jerry_release_value(js_name);
                    jerry_release_value(js_prop_val);
                }
            }
            jerryx_set_property_str(js_style, "attrs", js_attrs);

            jerry_release_value(js_attrs);
            jerry_release_value(js_has_group);
            jerry_release_value(js_selector);
            jerry_release_value(js_id);
            jerry_release_value(js_style);
        }

        jerry_release_value(styles);
        jerry_release_value(js_children);
        jerry_release_value(cur_id);
        jerry_release_value(cur);
    }

    jerry_value_t json = jerry_json_stringify(root);
    char *str = jerry_to_c_string(json);
    printf("%s\n", str);
    jerry_free_c_string(str);
    jerry_release_value(json);

    jerry_release_value(queue);
    jerry_release_value(root);
}