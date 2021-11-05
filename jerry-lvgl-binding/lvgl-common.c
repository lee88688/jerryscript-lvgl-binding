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