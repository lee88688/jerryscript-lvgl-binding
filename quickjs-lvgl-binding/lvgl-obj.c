#include "lvgl.h"
#include "quickjs.h"

JSValue js_lvgl_obj_constructor(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags) {
    if (flags & JS_CALL_FLAG_CONSTRUCTOR == 0) return JS_NULL;
    return this_val;
}

void js_lvgl_obj_finalizer(JSRuntime *rt, JSValue val) {}

static JSClassID js_lvgl_obj_class_id;

static JSClassDef js_lvgl_obj_class = {
    "LvglObj",
    .finalizer = js_lvgl_obj_finalizer,
    .call = js_lvgl_obj_constructor,
};

int js_lvgl_obj_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_obj_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_obj_class_id, &js_lvgl_obj_class);

    return 0;
}

