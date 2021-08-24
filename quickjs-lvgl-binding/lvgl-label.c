#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "lvgl-label.h"
#include "quickjs-lvgl-binding.h"

static JSClassID js_lvgl_label_class_id;

void js_lvgl_label_finalizer(JSRuntime *rt, JSValue val) {
    BI_LOG_TRACE("delete label\n");
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_label_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_label_class = {
    "LvglLabel",
    .finalizer = js_lvgl_label_finalizer,
};

static const JSCFunctionListEntry js_lvgl_label_proto_funcs[] = {
};

JSValue create_lvgl_label(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    CREATE_COMMON_LVGL_OBJ("label", lv_label_create, lv_scr_act(), ctx, js_lvgl_label_class_id, js_val);
    return js_val;
}

int js_lvgl_label_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_label_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_label_class_id, &js_lvgl_label_class);
    JSValue proto = JS_NewObjectClass(ctx, get_obj_class_id());
    JS_SetPropertyFunctionList(ctx, proto, js_lvgl_label_proto_funcs, countof(js_lvgl_label_proto_funcs));
    JS_SetPropertyStr(ctx, proto, "_class_id", JS_MKVAL(JS_TAG_INT, js_lvgl_label_class_id));
    JS_SetClassProto(ctx, js_lvgl_label_class_id, proto);

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "createLvglLabel", JS_NewCFunction(ctx, create_lvgl_label, "", 0));
    JS_FreeValue(ctx, global);

    return 0;
}
