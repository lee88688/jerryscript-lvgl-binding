---
to: ../quickjs-lvgl-binding/<%= 'lvgl-' + h.changeCase.lower(widgetName) %>.c
---
#include "lvgl.h"
#include "quickjs.h"
#include "cutils.h"
#include "lvgl-<%= h.changeCase.lower(widgetName) %>.h"
#include "quickjs-lvgl-binding.h"

static JSClassID js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id;

void js_lvgl_<%= h.changeCase.lower(widgetName) %>_finalizer(JSRuntime *rt, JSValue val) {
    BI_LOG_TRACE("delete <%= h.changeCase.lower(widgetName) %>\n");
    lv_obj_t *obj = JS_GetOpaque(val, js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id);
    lv_obj_del(obj);
}

static JSClassDef js_lvgl_<%= h.changeCase.lower(widgetName) %>_class = {
    "Lvgl<%= h.capitalize(widgetName) %>",
    .finalizer = js_lvgl_<%= h.changeCase.lower(widgetName) %>_finalizer,
};

static const JSCFunctionListEntry js_lvgl_<%= h.changeCase.lower(widgetName) %>_proto_funcs[] = {
};

JSValue create_lvgl_<%= h.changeCase.lower(widgetName) %>(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    CREATE_COMMON_LVGL_OBJ("<%= h.changeCase.lower(widgetName) %>", lv_<%= h.changeCase.lower(widgetName) %>_create, lv_scr_act(), ctx, js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id, js_val);
    return js_val;
}

int js_lvgl_<%= h.changeCase.lower(widgetName) %>_init(JSContext *ctx) {
    JS_NewClassID(&js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id, &js_lvgl_<%= h.changeCase.lower(widgetName) %>_class);
    JSValue proto = JS_NewObjectClass(ctx, get_obj_class_id());
    JS_SetPropertyFunctionList(ctx, proto, js_lvgl_<%= h.changeCase.lower(widgetName) %>_proto_funcs, countof(js_lvgl_<%= h.changeCase.lower(widgetName) %>_proto_funcs));
    JS_SetPropertyStr(ctx, proto, "_class_id", JS_MKVAL(JS_TAG_INT, js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id));
    JS_SetClassProto(ctx, js_lvgl_<%= h.changeCase.lower(widgetName) %>_class_id, proto);

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "createLvgl<%= h.capitalize(widgetName) %>", JS_NewCFunction(ctx, create_lvgl_<%= h.changeCase.lower(widgetName) %>, "", 0));
    JS_FreeValue(ctx, global);

    return 0;
}
