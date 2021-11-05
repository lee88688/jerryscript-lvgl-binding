#include <stdint.h>
#include <stdio.h>
#include "lvgl.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

// static jerry_value_t js_console_log(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
//     if (argc == 0) return jerry_create_undefined();

//     uint32_t i = 0;
//     for (; i < argc; i++) {
//         jerry_value_t str_value = jerry_value_to_string(argv[i]);
//         char *c_str = jerry_to_c_string(str_value);
//         printf("%s ", c_str);
//         jerry_free_c_string(str_value);
//         jerry_release_value(str_value);
//     }
//     printf("\n");
// }

static jerry_value_t js_console_warn(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    printf("[Warn] ");
    jerryx_handler_print(info, argv, argc);

    return jerry_create_undefined();
}

static jerry_value_t js_attach_to_screen(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    lv_obj_t *obj = NULL;
    if (js_lvgl_get_native_info(argv[0], (void **) &obj)) {
        lv_obj_set_parent(obj, lv_scr_act());
    }

    return jerry_create_undefined();
}

static const jerry_function_entry js_console_prototype_methods[] = {
    JERRY_CFUNC_DEF("log", jerryx_handler_print),
    JERRY_CFUNC_DEF("warn", js_console_warn),
};

static const jerry_function_entry js_global_prototype_methods[] = {
    JERRY_CFUNC_DEF("attachToScreen", js_attach_to_screen),
};

void js_runtime_init() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t console = jerry_create_object();
    
    jerryx_set_property_str(global, "console", console);
    jerry_set_prop_list(console, js_console_prototype_methods, countof(js_console_prototype_methods));
    jerry_set_prop_list(global, js_global_prototype_methods, countof(js_global_prototype_methods));

    jerry_release_value(console);
    jerry_release_value(global);
}

void js_event_loop() {
    jerry_value_t job_value;

    jerry_value_t g = jerry_get_global_object();
    jerry_value_t fn = jerryx_get_property_str(g, "execTimeoutFn");
    bool is_undefined = jerry_value_is_undefined(fn);
    jerry_release_value(fn);
    jerry_release_value(g);
    if (is_undefined) {
        BI_LOG_TRACE("event loop not ready!");
        return;
    }

    while (true) {
        job_value = jerry_run_all_enqueued_jobs();
        if (jerry_value_is_error(job_value)) {
            // get error, this means not unfinished
            BI_LOG_TRACE("microtask has error.");
            if (jerry_value_is_abort(job_value)) {
                jerry_port_fatal(jerry_value_as_uint32(job_value));
            }
            jerry_value_t error = jerry_get_value_from_error(job_value, false);
            jerry_value_t error_str = jerry_value_to_string(error);
            
            char *str = jerry_to_c_string(error_str);
            jerry_port_log(JERRY_LOG_LEVEL_ERROR, "%s", str);
            jerry_free_c_string(str);
            
            jerry_release_value(error_str);
            jerry_release_value(error);
        } else {
            // microtask is done, then execute macrotask(setTimeout)
            // when macrotask is empty, exit event loop to give cpu time to lvgl
            // BI_LOG_TRACE("start macrotask.");
            jerry_value_t global = jerry_get_global_object();
            jerry_value_t exec_timeout_fn = jerryx_get_property_str(global, "execTimeoutFn");
            jerry_value_t ret = jerry_call_function(exec_timeout_fn, global, NULL, 0);
            // timeout queue is empty
            if (!jerry_value_to_boolean(ret)) {
                // BI_LOG_TRACE("macrotask is empty.");
                jerry_release_value(job_value);
                break;
            }

            jerry_release_value(ret);
            jerry_release_value(exec_timeout_fn);
            jerry_release_value(global);
        }
        jerry_release_value(job_value);
    }
}