#include "lvgl-common.h"

int js_lvgl_get_ref(JSValue value) {
    int *ref_count = JS_VALUE_GET_PTR(value);
    return *ref_count;
}

JSValue js_lvgl_get_class_id(JSContext *ctx, JSValue value) {
    JSValue js_class_id = JS_GetPropertyStr(ctx, value, "_class_id");
    if (JS_IsUndefined(js_class_id)) {
        BI_LOG_WARN("get _class_id with undefined!\n")
        return JS_UNDEFINED;
    }
    if (JS_VALUE_GET_TAG(js_class_id) == JS_TAG_INT) {
        // js_class_id is a primitive type, it don't need free.
        return js_class_id;
    }
    JS_FreeValue(ctx, js_class_id);
    return JS_UNDEFINED;
}

lv_obj_t *js_lvgl_get_obj_opaque(JSContext *ctx, JSValue value) {
    JSValue class_id = js_lvgl_get_class_id(ctx, value);
    if (JS_IsUndefined(class_id)) {
        return NULL;
    }
    JSClassID id = JS_VALUE_GET_INT(class_id);
    lv_obj_t *obj = JS_GetOpaque(value, id);
    return obj;
}