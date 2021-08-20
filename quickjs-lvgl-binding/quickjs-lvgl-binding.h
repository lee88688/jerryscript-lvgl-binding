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

#endif /* QUICKJS_LVGL_BINGDING */