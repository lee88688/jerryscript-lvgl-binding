#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

// jerry_value_t setWidth(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
//   printf ("setWidth\n");
//   lv_obj_t *obj = NULL;
//   if (argc != 0 && jerry_value_is_number(argv[0]) && jerry_get_object_native_pointer(info->this_value, &obj, &lvgl_obj_native_info)) {
//       lv_coord_t value = jerry_value_as_uint32(argv[0]);
//       lv_obj_set_width(obj, value);
//   }

//   return jerry_create_undefined();
// }

static void lvgl_obj_free_cb (void *native_p, jerry_object_native_info_t *info_p)
{
    printf("deconstruct\n");
    lv_obj_t *obj = (lv_obj_t *) native_p;
    lv_obj_del(obj);
}

static const jerry_object_native_info_t lvgl_obj_native_info = {
    .free_cb = lvgl_obj_free_cb,
};

#include "obj-style-gen.c"

// STYLE_FUNC(width, lvgl_obj_native_info, 0);
// STYLE_FUNC(height, lvgl_obj_native_info, 0);


// static const jerry_function_entry js_obj_prototype_methods[] = {
//     JERRY_CFUNC_DEF("setWidth", js_lvgl_obj_set_width),
//     JERRY_CFUNC_DEF("setHeight", js_lvgl_obj_set_height),
// };

static jerry_value_t lvgl_obj_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    jerry_set_object_native_pointer(
        info->this_value,
        obj,
        &lvgl_obj_native_info
    );
    return jerry_create_undefined();
}

/**
 * 测试代码能不能跑过
 * free_cb在gc的时候有没有调用
 * 能不能正确生成一个对象
 */
void js_lvgl_obj_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t constructor = jerry_create_external_function(lvgl_obj_constructor);
    jerry_value_t proto = jerry_create_object();
    jerry_set_prop_list(proto, js_obj_prototype_methods, countof(js_obj_prototype_methods));
    jerryx_set_property_str(constructor, "prototype", proto);

    jerryx_set_property_str(global, "LvglObj", constructor);

    jerry_release_value(global);
    jerry_release_value(constructor);
    jerry_release_value(proto);
}
