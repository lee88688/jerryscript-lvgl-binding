#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "lvgl-label.h"
#include "lvgl-common.h"

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

JSValue js_lvgl_label_set_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc == 0) return JS_UNDEFINED;
    JSValue js_class_id = js_lvgl_get_class_id(ctx, this_val);
    if (JS_IsUndefined(js_class_id)) {
        return JS_UNDEFINED;
    }
    JSClassID id = JS_VALUE_GET_INT(js_class_id);
    lv_obj_t *obj = JS_GetOpaque(this_val, id);
    const char *str = JS_ToCString(ctx, *argv);
    lv_label_set_text(obj, str);
    JS_FreeCString(ctx, str);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_lvgl_label_proto_funcs[] = {
    JS_CFUNC_DEF("setText", 1, js_lvgl_label_set_text),
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
