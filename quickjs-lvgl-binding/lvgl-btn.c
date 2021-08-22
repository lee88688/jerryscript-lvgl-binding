#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "lvgl-obj.h"
#include "quickjs-lvgl-binding.h"

static JSClassID js_lvgl_btn_class_id;

void js_lvgl_btn_finalizer(JSRuntime *rt, JSValue val) {
    printf("delete btn\n");
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_btn_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_btn_class = {
    "LvglBtn",
    .finalizer = js_lvgl_btn_finalizer,
    // .call = js_lvgl_btn_constructor,
};

static const JSCFunctionListEntry js_lvgl_btn_proto_funcs[] = {
};

JSValue create_lvgl_btn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    CREATE_COMMON_LVGL_OBJ("obj", lv_btn_create, lv_scr_act(), ctx, js_lvgl_btn_class_id, js_val);
    return js_val;
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
