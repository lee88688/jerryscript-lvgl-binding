#ifndef LVGL_COMMON_H
#define LVGL_COMMON_H

#include "lvgl.h"
#include "jerryscript-core.h"

bool js_lvgl_get_native_info(jerry_value_t value, void **obj);

#define countof(x) (sizeof(x) / sizeof((x)[0]))

void _bi_log(lv_log_level_t level, const char * file, int line, const char * func, const char * format, ...);
#define BI_LOG_LEVEL LV_LOG_LEVEL_TRACE

#if BI_LOG_LEVEL <= LV_LOG_LEVEL_TRACE
#define BI_LOG_TRACE(...) _bi_log(LV_LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define BI_LOG_TRACE(...)
#endif

#if BI_LOG_LEVEL <= LV_LOG_LEVEL_INFO
#define BI_LOG_INFO(...) _bi_log(LV_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define BI_LOG_INFO(...)
#endif

#if BI_LOG_LEVEL <= LV_LOG_LEVEL_WARN
#define BI_LOG_WARN(...) _bi_log(LV_LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define BI_LOG_WARN(...)
#endif

#if BI_LOG_LEVEL <= LV_LOG_LEVEL_ERROR
#define BI_LOG_ERROR(...) _bi_log(LV_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define BI_LOG_ERROR(...)
#endif

typedef struct jerry_function_entry {
    char *name;
    uint8_t def_type;
    union {
        jerry_external_handler_t func;
    } u;
} jerry_function_entry;
// entry type
#define JERRY_DEF_CFUNC          0
#define JERRY_DEF_CGETSET        1
#define JERRY_DEF_CGETSET_MAGIC  2
#define JERRY_DEF_PROP_STRING    3
#define JERRY_DEF_PROP_INT32     4
#define JERRY_DEF_PROP_DOUBLE    6
#define JERRY_DEF_PROP_UNDEFINED 7
#define JERRY_DEF_OBJECT         8
// entry define type
#define JERRY_CFUNC_DEF(name, cfunc) { name, .def_type=JERRY_DEF_CFUNC, .u={ cfunc } }

// result error handle, if error log error
#define JERRY_ERROR_HANDLE(result, msg) \
    if (jerry_value_is_error(result)) { \
        BI_LOG_WARN("%s", msg); \
    }

/**
 * type_align
 * 0 - normal type for uint like type
 * 1 - color type for lv_color_t
 */
#define STYLE_TYPE_DEF_0(param) \
    uint32_t value = jerry_value_as_uint32(param);

#define STYLE_TYPE_DEF_1(param) \
    uint32_t res = jerry_value_as_uint32(param); \
    lv_color_t value = LV_COLOR_MAKE(res >> 16 & 0xff, res >> 8 & 0xff, res & 0xff);


#define STYLE_FUNC(opt_name, type_align) \
    jerry_value_t js_lvgl_obj_set_##opt_name(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) { \
        BI_LOG_TRACE("set style: %s", #opt_name); \
        void *obj = NULL; \
        lv_style_selector_t selector = 0; \
        if (argc == 2 && jerry_value_is_number(argv[1])) { \
            selector = jerry_value_as_integer(argv[1]); \
        } \
        if ( \
            argc != 0 \
            && jerry_value_is_number(argv[0]) \
            && js_lvgl_get_native_info(info->this_value, &obj) \
        ) { \
            STYLE_TYPE_DEF_##type_align(argv[0]); \
            lv_obj_set_style_##opt_name(obj, value, selector); \
            BI_LOG_TRACE("set style complete: %s", #opt_name); \
        } \
        return jerry_create_undefined(); \
    }

#define NATIVE_INFO_PROP_STR "_native_info"

void jerry_set_prop_list(jerry_value_t value, const jerry_function_entry *entries, jerry_size_t len);

char *jerry_to_c_string(jerry_value_t value);
void jerry_free_c_string(char *str);

lv_obj_t *js_lvgl_get_detach_screen();
void js_lvgl_detach_children(lv_obj_t *parent);

#endif /* LVGL_COMMON_H */