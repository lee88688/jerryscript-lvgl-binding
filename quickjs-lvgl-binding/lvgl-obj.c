#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "quickjs-lvgl-binding.h"
#include "obj-style-gen.c"

static JSClassID js_lvgl_obj_class_id;

static void js_lvgl_obj_finalizer(JSRuntime *rt, JSValue val) {
    printf("finalizer");
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_obj_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_obj_class = {
    "LvglObj",
    .finalizer = js_lvgl_obj_finalizer,
    // .call = js_lvgl_obj_constructor,
};

static JSValue create_lvgl_obj(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    CREATE_COMMON_LVGL_OBJ("obj", lv_obj_create, lv_scr_act(), ctx, js_lvgl_obj_class_id, js_val);
    return js_val;
}

int js_lvgl_obj_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_obj_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_obj_class_id, &js_lvgl_obj_class);
    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_lvgl_obj_proto_funcs, countof(js_lvgl_obj_proto_funcs));
    JS_SetPropertyStr(ctx, proto, "_class_id", JS_MKVAL(JS_TAG_INT, js_lvgl_obj_class_id)); // this is used for get opaque value from subclass
    JS_SetClassProto(ctx, js_lvgl_obj_class_id, proto);

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "createLvglObj", JS_NewCFunction(ctx, create_lvgl_obj, "", 0));
    JS_FreeValue(ctx, global);

    return 0;
}

JSClassID get_obj_class_id() {
    return js_lvgl_obj_class_id;
}
