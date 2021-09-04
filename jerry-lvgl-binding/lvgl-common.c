#include <stdio.h>
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

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