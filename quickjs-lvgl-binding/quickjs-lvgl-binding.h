#ifndef QUICKJS_LVGL_BINGDING_H
#define QUICKJS_LVGL_BINGDING_H

#include "lvgl-obj.h"
#include "lvgl-btn.h"

#define CREATE_COMMON_LVGL_OBJ(name, create_func, parent, ctx, class_id, ret_val) \
    lv_obj_t *obj = create_func(parent); \
    if (!obj) { \
        return JS_ThrowInternalError(ctx, "create obj fail!"); \
    } \
    JSValue ret_val = JS_NewObjectClass(ctx, class_id); \
    if (JS_IsException(ret_val)) { \
        return ret_val; \
    } \
    JS_SetOpaque(ret_val, obj); \
    printf("create %s\n", name);


/**
 * type_align
 * 0 - normal type for uint like type
 * 1 - color type for lv_color_t
 */
#define STYLE_TYPE_DEF_0(js_tag_type, param) \
    uint32_t value = JS_VALUE_GET_##js_tag_type(param);

#define STYLE_TYPE_DEF_1(js_tag_type, param) \
    uint32_t res = JS_VALUE_GET_##js_tag_type(param); \
    lv_color_t value = LV_COLOR_MAKE(res >> 16 & 0xff, res >> 8 & 0xff, res & 0xff);

#define STYLE_FUNC(opt_name, js_tag_type, type_align) \
    static JSValue js_lvgl_obj_set_##opt_name(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { \
        printf("set %s\n", #opt_name); \
        if (argc == 0) return JS_UNDEFINED; \
        lv_style_selector_t selector = 0; \
        if (argc == 2 && JS_VALUE_GET_TAG(argv[1]) != JS_TAG_INT) { \
            selector = JS_VALUE_GET_INT(argv[1]); \
        } \
        if (JS_VALUE_GET_TAG(*argv) != JS_TAG_##js_tag_type) return JS_UNDEFINED; \
        JSValue proto = JS_GetPrototype(ctx, this_val); \
        JSValue js_class_id = JS_GetPropertyStr(ctx, proto, "_class_id"); \
        JS_FreeValue(ctx, proto); \
        if (JS_IsUndefined(js_class_id)) { \
            printf("get _class_id with undefined!\n"); \
            return JS_UNDEFINED; \
        } \
        lv_obj_t *obj = JS_GetOpaque(this_val, (JSClassID) JS_VALUE_GET_INT(js_class_id)); \
        JS_FreeValue(ctx, js_class_id); \
        if (!obj) { \
            return JS_UNDEFINED; \
        } \
        STYLE_TYPE_DEF_##type_align(js_tag_type, *argv) \
        lv_obj_set_style_##opt_name(obj, value, selector); \
        printf("set style: %s\n", #opt_name); \
        return JS_UNDEFINED; \
    }

JSValue print_mem_info(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif /* QUICKJS_LVGL_BINGDING */