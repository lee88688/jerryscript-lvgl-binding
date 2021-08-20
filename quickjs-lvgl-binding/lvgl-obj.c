#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"

static JSClassID js_lvgl_obj_class_id;

static JSValue js_lvgl_obj_set_width(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc == 0) return JS_UNDEFINED;

    if (JS_VALUE_GET_TAG(*argv) != JS_TAG_INT) return JS_UNDEFINED;
    lv_obj_t *obj = JS_GetOpaque(this_val, js_lvgl_obj_class_id);
    printf("obj: %d\n", obj);
    lv_obj_set_width(obj, JS_VALUE_GET_INT(*argv));

    return JS_UNDEFINED;
}

static JSValue js_lvgl_obj_set_height(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc == 0) return JS_UNDEFINED;
    if (JS_VALUE_GET_TAG(*argv) != JS_TAG_INT) return JS_UNDEFINED;

    lv_obj_t *obj = JS_GetOpaque(this_val, js_lvgl_obj_class_id);
    lv_obj_set_height(obj, JS_VALUE_GET_INT(*argv));
    printf("set height: %d\n", JS_VALUE_GET_INT(*argv));

    return JS_UNDEFINED;
}

JSValue js_lvgl_obj_constructor(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags) {
    printf("flags: %d\n", flags);
    if ((flags & JS_CALL_FLAG_CONSTRUCTOR) == 0) return JS_NULL;
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    JS_SetOpaque(this_val, obj);
    printf("create obj: %d\n", obj);
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

static const JSCFunctionListEntry js_lvgl_obj_proto_funcs[] = {
    JS_CFUNC_DEF("setWidth", 1, js_lvgl_obj_set_width),
    JS_CFUNC_DEF("setHeight", 1, js_lvgl_obj_set_height),
};

static JSValue create_lvgl_obj(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    return JS_NewObjectClass(ctx, js_lvgl_obj_class_id);
}

int js_lvgl_obj_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_obj_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_obj_class_id, &js_lvgl_obj_class);
    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_lvgl_obj_proto_funcs, countof(js_lvgl_obj_proto_funcs));
    JS_SetClassProto(ctx, js_lvgl_obj_class_id, proto);

    JSValue global = JS_GetGlobalObject(ctx);
    JSValue Obj = JS_NewObjectClass(ctx, js_lvgl_obj_class_id);
    JS_SetConstructorBit(ctx, Obj, TRUE);
    JS_SetPropertyStr(ctx, global, "createLvglObj", JS_NewCFunction(ctx, create_lvgl_obj, "", 0));
    // JS_SetPropertyStr(ctx, global, "Obj", Obj);
    JS_FreeValue(ctx, Obj);
    JS_FreeValue(ctx, Obj);
    // printf("before string count: %d\n", **cnt);
    JS_FreeValue(ctx, global);

    return 0;
}

JSClassID get_obj_class_id() {
    return js_lvgl_obj_class_id;
}
