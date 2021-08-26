#include <string.h>
// #include <emscripten.h>
#include "quickjs.h"
#include "lvgl.h"
#include "quickjs-lvgl-binding.h"

static JSValue checkCount(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    // int **cnt = (void **) argv;
    // printf("before string count: %d\n", **cnt);
    // JS_FreeValue(ctx, *argv);
    // printf("after string count: %d\n", **cnt);
    JS_FreeValue(ctx, *argv);
    JS_RunGC(JS_GetRuntime(ctx));
    return JS_NULL;
}

static JSValue addButton(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    lv_obj_t *obj = lv_btn_create(lv_scr_act());
    lv_obj_set_height(obj, LV_SIZE_CONTENT);

    return JS_UNDEFINED;
}

static JSValue log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc == 0) return JS_UNDEFINED;
    const char *str = JS_ToCString(ctx, *argv);
    if (str == NULL) return JS_UNDEFINED;
    printf("%s\n", str);
    JS_FreeCString(ctx, str);

    return JS_UNDEFINED;
}

static JSRuntime* runtime = NULL;
static JSContext* ctx = NULL;

const char* eval(const char* str) {
	if (!runtime) {
        runtime = JS_NewRuntime();
        ctx = JS_NewContext(runtime);
        JSValue global = JS_GetGlobalObject(ctx);
        JS_SetPropertyStr(ctx, global, "addButton", JS_NewCFunction(ctx, addButton, "addButton", 1));
        JS_SetPropertyStr(ctx, global, "log", JS_NewCFunction(ctx, log, "", 1));
        JS_FreeValue(ctx, global);
        quickjs_lvgl_binding_init(ctx);
    }
	JSValue result =
	    JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(result)) {
		JSValue realException = JS_GetException(ctx);
		return JS_ToCString(ctx, realException);
	}
	JSValue json = JS_JSONStringify(ctx, result, JS_UNDEFINED, JS_UNDEFINED);
	JS_FreeValue(ctx, result);
    JS_RunGC(runtime);
	const char* ret = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);
    return ret;
}