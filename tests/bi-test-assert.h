#ifndef BI_TEST_ASSERT_H
#define BI_TEST_ASSERT_H

#include <stdint.h>
#include "lvgl-common.h"

void bi_test_assert_true(int32_t expression, const char *expression_str, const char * file, int line, const char * s);

#define BI_ASSERT_TRUE(expression, message) bi_test_assert_true(expression, #expression, __FILE__, __LINE__, message)

#define BI_TEST_START BI_LOG_INFO("TEST START")
#define BI_TEST_END BI_LOG_INFO("TEST END")

#endif