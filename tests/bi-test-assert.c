#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "jerryscript.h"

void bi_test_print(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stdout, s, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void bi_test_exit(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);

    jerry_port_fatal(ERR_FAILED_INTERNAL_ASSERTION);
}

void bi_test_error(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    jerry_port_fatal(ERR_FAILED_INTERNAL_ASSERTION);
}

void bi_test_assert(const char * file, int line, const char * func, const char * format, ...) {}

void bi_test_assert_true(int32_t expression, const char *expression_str, const char * file, int line, const char * s)
{
    if(!expression) {
        bi_test_error("   FAIL: %s. expression: %s(Expected: true), at file(%s) line(%d)\n", s, expression_str, file, line);
        jerry_port_fatal(ERR_FAILED_INTERNAL_ASSERTION);
    } else {
        // bi_test_print("   PASS: %s. (Expected: not false)", s, expression);
    }
}
