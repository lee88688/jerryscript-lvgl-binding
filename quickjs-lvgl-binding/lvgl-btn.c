#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "lvgl-obj.h"

static JSClassID js_lvgl_btn_class_id;

JSValue js_lvgl_btn_constructor(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags) {
    if ((flags & JS_CALL_FLAG_CONSTRUCTOR) == 0) return JS_NULL;
    lv_obj_t *obj = lv_btn_create(lv_scr_act());
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    printf("create btn\n");

    JS_SetOpaque(this_val, obj);
    return this_val;
}

void js_lvgl_btn_finalizer(JSRuntime *rt, JSValue val) {
    printf("delete btn\n");
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_btn_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_btn_class = {
    "LvglBtn",
    .finalizer = js_lvgl_btn_finalizer,
    .call = js_lvgl_btn_constructor,
};

static const JSCFunctionListEntry js_lvgl_btn_proto_funcs[] = {
};

JSValue create_lvgl_btn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    printf("create btn\n");
    return JS_NewObjectClass(ctx, js_lvgl_btn_class_id);
}

int js_lvgl_btn_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_btn_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_btn_class_id, &js_lvgl_btn_class);
    JSValue proto = JS_NewObjectClass(ctx, get_obj_class_id());
    JS_SetPropertyFunctionList(ctx, proto, js_lvgl_btn_proto_funcs, countof(js_lvgl_btn_proto_funcs));
    JS_SetClassProto(ctx, js_lvgl_btn_class_id, proto);

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "createLvglBtn", JS_NewCFunction(ctx, create_lvgl_btn, "", 0));
    JS_FreeValue(ctx, global);

    return 0;
}
