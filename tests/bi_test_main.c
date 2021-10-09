// #include <lvgl/lvgl.h>
#include <stdio.h>
#include "test_core/test_core.h"

void bi_test_main() {
    test_append_child();
    test_remove_child();
    test_insert_before_without_append();
    test_insert_before_with_append();
}