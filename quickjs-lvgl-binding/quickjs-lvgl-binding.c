#include "quickjs.h"
#include "lvgl.h"
#include "quickjs-lvgl-binding.h"
#include "lvgl-obj.h"
#include "lvgl-btn.h"
#include "lvgl-label.h"

JSValue print_mem_info(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    JSMemoryUsage *mem_usage = js_malloc(ctx, sizeof(JSMemoryUsage));
    JS_ComputeMemoryUsage(JS_GetRuntime(ctx), mem_usage);

    printf("malloc size: %lld, limit: %lld, used size: %lld.\n", mem_usage->malloc_size, mem_usage->malloc_limit, mem_usage->memory_used_size);
    printf("malloc count: %lld.\n", mem_usage->malloc_count);
    printf("malloc used count: %lld.\n", mem_usage->memory_used_count);
    printf("atom count: %lld, atom size: %lld.\n", mem_usage->atom_count, mem_usage->atom_size);
    printf("str count: %lld, str size: %lld.\n", mem_usage->str_count, mem_usage->str_size);
    printf("obj count: %lld, obj size: %lld.\n", mem_usage->obj_count, mem_usage->obj_size);
    printf("prop count: %lld, prop size: %lld.\n", mem_usage->prop_count, mem_usage->prop_size);
    printf("shape count: %lld, shape size: %lld.\n", mem_usage->shape_count, mem_usage->shape_size);
    printf("js func count: %lld, js func size: %lld, js code size: %lld.\n", mem_usage->js_func_count, mem_usage->js_func_size, mem_usage->js_func_code_size);
    printf("js func pc2line count: %lld, js func pc2line size: %lld.\n", mem_usage->js_func_pc2line_count, mem_usage->js_func_pc2line_size);
    printf("c_func_count count: %lld, c_func_count size: %lld.\n", mem_usage->c_func_count, mem_usage->array_count);
    printf("fast array count: %lld, fast array elements size: %lld.\n", mem_usage->fast_array_count, mem_usage->fast_array_elements);
    printf("binary obj count: %lld, binary obj size: %lld.\n", mem_usage->binary_object_count, mem_usage->binary_object_size);

    js_free(ctx, mem_usage);
    return JS_UNDEFINED;
}

void quickjs_lvgl_binding_init(JSContext *ctx) {
    JSValue global = JS_GetGlobalObject(ctx);

    JS_SetPropertyStr(ctx, global, "print_mem_info", JS_NewCFunction(ctx, print_mem_info, "print_mem_info", 0));

    js_lvgl_obj_init(ctx);
    js_lvgl_btn_init(ctx);
    js_lvgl_label_init(ctx);
    JS_FreeValue(ctx, global);
}
