#include <string.h>
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"
#include "jerry-lvgl-binding.h"
#include "../bi-test-assert.h"

void test_lvgl_common_utf8_string() {
    BI_TEST_START;
    jerry_init(JERRY_INIT_EMPTY);

    js_runtime_init();

    char A[] = "Á";
    jerry_value_t value = jerry_create_string_from_utf8((const jerry_char_t *) A);
    jerry_char_t *str = jerry_to_c_string(value);

    BI_ASSERT_TRUE(memcmp(A, str, sizeof(A) - 1) == 0, "");
    jerry_free_c_string(str);

    // jerry_value_t global = jerry_get_global_object();
    // jerryx_set_property_str(global, "A", value);
    // str = NULL;
    // char code[] = "console.log(A.charCodeAt(0));";
    // jerry_value_t ret = jerry_eval(code, sizeof(code) - 1, JERRY_PARSE_NO_OPTS);

    // BI_ASSERT_TRUE(!jerry_value_is_error(ret), str = jerry_to_c_string(jerry_get_value_from_error(ret, true)));
    // jerry_free_c_string(str);
    // jerry_release_value(global);

    jerry_release_value(value);

    jerry_cleanup();
    BI_TEST_END;
}