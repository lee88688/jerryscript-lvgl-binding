#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"

static JSClassID js_lvgl_obj_class_id;

static JSValue js_lvgl_obj_set_width(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc == 0) return JS_UNDEFINED;
    if (JS_VALUE_GET_TAG(this_val) != JS_TAG_INT) return JS_UNDEFINED;
    lv_obj_t *obj = JS_GetOpaque(this_val, js_lvgl_obj_class_id);
    lv_obj_set_width(obj, JS_VALUE_GET_INT(*argv));

    return JS_UNDEFINED;
}

JSValue js_lvgl_obj_constructor(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags) {
    if ((flags & JS_CALL_FLAG_CONSTRUCTOR) == 0) return JS_NULL;
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    JS_SetOpaque(this_val, obj);
    return this_val;
}

void js_lvgl_obj_finalizer(JSRuntime *rt, JSValue val) {
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_obj_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_obj_class = {
    "LvglObj",
    .finalizer = js_lvgl_obj_finalizer,
    .call = js_lvgl_obj_constructor,
};

int js_lvgl_obj_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_obj_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_obj_class_id, &js_lvgl_obj_class);
    JSValue proto = JS_NewObject(ctx);
    JS_SetClassProto(ctx, js_lvgl_obj_class_id, proto);

    return 0;
}

