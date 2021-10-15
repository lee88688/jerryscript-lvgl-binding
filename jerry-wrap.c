#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "emscripten/websocket.h"
#include "jerryscript.h"
#include "jerryscript-ext/debugger.h"
#include "jerry-lvgl-binding.h"

static char buff[100] = {0};

const char *eval(const char *str) {
    static int is_init = 0;
    if (!is_init) {
        jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "jerry log\n");
        jerry_init(JERRY_INIT_EMPTY);

        js_lvgl_obj_init();
        js_lvgl_label_init();
        js_runtime_init();
    }

    const jerry_length_t script_len = strlen(str);
    jerry_value_t eval_ret = jerry_eval(str, script_len, JERRY_PARSE_NO_OPTS);
    JERRY_ERROR_HANDLE(eval_ret, "eval with error!");
    if (jerry_value_is_error(eval_ret)) {
        eval_ret = jerry_get_value_from_error(eval_ret, true);
        printf("get error from value!\n");
    }
    jerry_value_t ret_str = jerry_value_to_string(eval_ret);
    jerry_string_to_char_buffer(ret_str, buff, sizeof(buff));
    printf("%s\n", buff);

    // jerry_release_value (eval_ret);
    // jerry_release_value (ret_str);
    // jerry_cleanup ();
    return buff;
}

void run(const char *str) {
    bool res = jerry_em_debugger_create() && jerryx_debugger_rp_create();
    printf("create %d\n", res);
    jerryx_debugger_after_connect(res);
    if (jerry_debugger_is_connected()) {
        printf("debugger is created.\n");
    } else {
        printf("fail to create debugger.\n");
    }

    const jerry_length_t script_len = strlen(str);
    // jerry_value_t eval_ret = jerry_eval(str, script_len, JERRY_PARSE_NO_OPTS);
    jerry_parse_options_t parseOption = { .options = JERRY_PARSE_NO_OPTS };
    jerry_value_t ret = jerry_parse(str, script_len, &parseOption);

    if (!jerry_value_is_error(ret)) {
        jerry_value_t parsed_code = ret;
        ret = jerry_run(parsed_code);
        jerry_release_value(parsed_code);
    }

    if (jerry_value_is_error(ret)) {
        ret = jerry_get_value_from_error(ret, true);
        printf("get an error!\n");
    }
    jerry_value_t ret_str = jerry_value_to_string(ret);
    char *s = jerry_to_c_string(ret_str);
    printf("%s\n", s);
    jerry_free_c_string(s);
}


// static const EmscriptenWebSocketCreateAttributes websocket_attr = {
//     .url = "ws://127.0.0.1:9001/",
//     .protocols = NULL,
//     .createOnMainThread = 1
// };


// void on_open_callback(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
//     emscripten_websocket_send_utf8_text(websocketEvent->socket, "hello");
// }

// void socket() { 
//     EMSCRIPTEN_WEBSOCKET_T soc = emscripten_websocket_new(&websocket_attr);
//     if (soc < 0) {
//         printf("websocket create failed!\n");
//     }
//     emscripten_websocket_set_onopen_callback(soc, "hello", on_open_callback);
// }