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
        jerry_length_t len = jerry_get_string_length(value);
        char *str = malloc(len + 1);
        jerry_string_to_char_buffer(value, str, len + 1);
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