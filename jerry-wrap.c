#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "jerryscript.h"

static char buff[100] = {0};

const char *eval(const char *str) {
    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "jerry log\n");
    const jerry_length_t script_len = strlen(str);
    jerry_init(JERRY_INIT_EMPTY);
    jerry_value_t eval_ret = jerry_eval(str, script_len, JERRY_PARSE_NO_OPTS);
    jerry_value_t ret_str = jerry_value_to_string(eval_ret);
    jerry_string_to_char_buffer(ret_str, buff, sizeof(buff));

    jerry_release_value (eval_ret);
    jerry_release_value (ret_str);
    jerry_cleanup ();
    return buff;
}