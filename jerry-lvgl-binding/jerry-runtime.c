#include <stdint.h>
#include <stdio.h>
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

static jerry_value_t js_console_log(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    uint32_t i = 0;
    for (; i < argc; i++) {
        jerry_value_t str_value = jerry_value_to_string(argv[i]);
        char *c_str = jerry_to_c_string(str_value);
        printf("%s ", c_str);
        jerry_free_c_string(str_value);
        jerry_release_value(str_value);
    }
    printf("\n");
}

static jerry_value_t js_console_warn(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    printf("[Warn] ");
    js_console_log(info, argv, argc);
}

static const jerry_function_entry js_obj_prototype_methods[] = {
    JERRY_CFUNC_DEF("log", js_console_log),
    JERRY_CFUNC_DEF("warn", js_console_warn),
};

void js_runtime_init() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t console = jerry_create_object();
    
    jerryx_set_property_str(global, "console", console);
    jerry_set_prop_list(console, js_obj_prototype_methods, countof(js_obj_prototype_methods));

    jerry_release_value(console);
    jerry_release_value(global);
}