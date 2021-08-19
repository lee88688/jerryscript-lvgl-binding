#include <string.h>
#include "quickjs.h"
#include "lvgl.h"
#include "lvgl-obj.h"

static JSValue addNewButton(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_height(btn, LV_SIZE_CONTENT);
    return JS_NULL;
}

static JSRuntime* runtime = NULL;
static JSContext* ctx = NULL;

const char* eval(const char* str) {
	if (!runtime) {
        runtime = JS_NewRuntime();
        ctx = JS_NewContext(runtime);
        JSValue global = JS_GetGlobalObject(ctx);
        js_lvgl_obj_init(ctx);
        JS_FreeValue(ctx, global);
    }
	JSValue result =
	    JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(result)) {
		JSValue realException = JS_GetException(ctx);
		return JS_ToCString(ctx, realException);
	}
	JSValue json = JS_JSONStringify(ctx, result, JS_UNDEFINED, JS_UNDEFINED);
	JS_FreeValue(ctx, result);
	const char* ret = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);
    return ret;
}