#include "jerryscript.h"
#include "lvgl.h"
#include "../bi_test_assert.h"
#include "lvgl-common.h"
#include "jerry-lvgl-binding.h"

void test_append_child() {
    BI_TEST_START;
    jerry_init(JERRY_INIT_EMPTY);

    js_lvgl_obj_init();

    const char code[] = "let a = new LvglObj(); let b = new LvglObj(); let c = new LvglObj(); a.appendChild(c); a.appendChild(b); [a, b];";
    jerry_value_t ret = jerry_eval(code, sizeof(code) - 1, JERRY_PARSE_NO_OPTS);

    char *str = NULL;
    BI_ASSERT_TRUE(!jerry_value_is_error(ret), str = jerry_to_c_string(jerry_get_value_from_error(ret, true)));
    jerry_free_c_string(str);

    BI_ASSERT_TRUE(jerry_value_is_array(ret), "eval result is not array!");

    jerry_value_t a = jerry_get_property_by_index(ret, 0);
    jerry_value_t b = jerry_get_property_by_index(ret, 1);
    lv_obj_t *a_obj = NULL;
    lv_obj_t *b_obj = NULL;
    if (js_lvgl_get_native_info(a, &a_obj) && js_lvgl_get_native_info(b, &b_obj)) {
        uint32_t index = lv_obj_get_child_id(b_obj);
        BI_ASSERT_TRUE(index == 1, "");
        BI_ASSERT_TRUE(b_obj->parent == a_obj, "");
    } else {
        BI_ASSERT_TRUE(false, "cannot get lvgl obj from js object!");
    }


    jerry_release_value(a);
    jerry_release_value(b);
    jerry_release_value(ret);

    jerry_cleanup();
    BI_TEST_END;
}