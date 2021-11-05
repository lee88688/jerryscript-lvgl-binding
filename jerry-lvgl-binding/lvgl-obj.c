#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"
#include "lvgl-style.h"

static const char *NAME = "LvglObj";

static void lvgl_obj_free_cb (void *native_p, jerry_object_native_info_t *info_p)
{
    BI_LOG_TRACE("deconstruct");
    lv_obj_t *obj = (lv_obj_t *) native_p;
    js_lvgl_detach_children(obj);
    lv_obj_del(obj);
}

static const jerry_object_native_info_t lvgl_obj_native_info = {
    .free_cb = lvgl_obj_free_cb,
};

#include "obj-style-gen.h"

jerry_value_t lvgl_obj_constructor(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_obj_t *obj = lv_obj_create(js_lvgl_get_detach_screen());
    jerry_set_object_native_pointer(
        info->this_value,
        obj,
        &lvgl_obj_native_info
    );
    return jerry_create_undefined();
}

jerry_value_t lvgl_obj_opt_styles(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc < 2) return jerry_create_undefined();

    // style obj can be array or single obj
    // argv[0] is type 0 for delete and 1 for add, argv[1] for styles
    lv_obj_t *obj = NULL;
    js_lvgl_get_native_info(info->this_value, (void **) &obj);

    uint32_t type = jerry_value_as_uint32(argv[0]);
    jerry_value_t obj_or_arr = argv[1];
    bool is_array = jerry_value_is_array(argv[1]);
    if (!is_array) {
        lv_style_t *style = NULL;
        bool have_native_style = jerry_get_object_native_pointer(obj_or_arr, (void **) &style, get_lvgl_style_native_info());
        if (have_native_style) {
            jerry_value_t selector = jerryx_get_property_str(obj_or_arr, "selector");
            if (type == 1) {
                lv_obj_add_style(obj, style, jerry_value_as_uint32(selector));
            } else {
                lv_obj_remove_style(obj, style, jerry_value_as_uint32(selector));
            }
            jerry_release_value(selector);
        }
    } else {
        jerry_value_t arr = obj_or_arr;
        uint32_t len = jerry_get_array_length(arr);
        uint32_t i = 0;
        for(i = 0; i < len; i++) {
            jerry_value_t v = jerry_get_property_by_index(arr, i);
            lv_style_t *style = NULL;
            bool have_native_style = jerry_get_object_native_pointer(v, (void **)&style, get_lvgl_style_native_info());
            if (have_native_style) {
                jerry_value_t selector = jerryx_get_property_str(v, "selector");
                if (type == 1) {
                    lv_obj_add_style(obj, style, jerry_value_as_uint32(selector));
                } else {
                    lv_obj_remove_style(obj, style, jerry_value_as_uint32(selector));
                }
                jerry_release_value(selector);
            }

            jerry_release_value(v);
        }
    }

    return jerry_create_undefined();
}

static jerry_value_t lvgl_obj_append_child(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    lv_obj_t *parent_obj = NULL;
    lv_obj_t *child_obj = NULL;
    if (js_lvgl_get_native_info(info->this_value, (void **)&parent_obj) && js_lvgl_get_native_info(argv[0], (void **) &child_obj)) {
        lv_obj_set_parent(child_obj, parent_obj);
    }

    return jerry_create_undefined();
}

static jerry_value_t lvgl_obj_remove_child(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc == 0) return jerry_create_undefined();

    lv_obj_t *child_obj = NULL;
    if (js_lvgl_get_native_info(argv[0], (void **)&child_obj)) {
        lv_obj_set_parent(child_obj, js_lvgl_get_detach_screen());
    }

    return jerry_create_undefined();
}

static jerry_value_t lvgl_obj_insert_before(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    if (argc < 2) return jerry_create_undefined();

    lv_obj_t *parent_obj = NULL;
    lv_obj_t *child_obj = NULL;
    lv_obj_t *before_obj = NULL;
    if (
        js_lvgl_get_native_info(info->this_value, (void **)&parent_obj)
        && js_lvgl_get_native_info(argv[0], (void **)&child_obj)
        && js_lvgl_get_native_info(argv[1], (void **)&before_obj)
    ) {
        // todo: assert before_index != 0xFFFFFFFF
        // insert child to parent and move it to before
        lv_obj_set_parent(child_obj, parent_obj);
        // todo: assert patent obj children length
        // uint32_t len = lv_obj_get_child_cnt(parent_obj);
        // if (len == 0) return jerry_create_undefined();
        uint32_t before_index = lv_obj_get_child_id(before_obj);
        uint32_t child_index = lv_obj_get_child_id(child_obj);
        for (; child_index > before_index; child_index--) {
            parent_obj->spec_attr->children[child_index] = parent_obj->spec_attr->children[child_index - 1];
        }
        parent_obj->spec_attr->children[before_index] = child_obj;
    } else {
        BI_LOG_WARN("get lvgl obj fail!");
    }

    return jerry_create_undefined();
}

static const jerry_function_entry js_obj_prototype_methods2[] = {
    JERRY_CFUNC_DEF("optObjStyles", lvgl_obj_opt_styles),
    JERRY_CFUNC_DEF("appendChild", lvgl_obj_append_child),
    JERRY_CFUNC_DEF("removeChild", lvgl_obj_remove_child),
    JERRY_CFUNC_DEF("insertBefore", lvgl_obj_insert_before),
};

void js_lvgl_obj_init() {
    jerry_value_t global = jerry_get_global_object();

    jerry_value_t constructor = jerry_create_external_function(lvgl_obj_constructor);
    jerry_value_t proto = jerry_create_object();
    jerry_set_prop_list(proto, js_obj_prototype_methods, countof(js_obj_prototype_methods));

    jerry_set_prop_list(proto, js_obj_prototype_methods2, countof(js_obj_prototype_methods2));

    jerry_value_t native_info = jerry_create_number((uint32_t) &lvgl_obj_native_info);
    jerryx_set_property_str(proto, NATIVE_INFO_PROP_STR, native_info);

    jerryx_set_property_str(constructor, "prototype", proto);

    jerryx_set_property_str(global, NAME, constructor);

    jerry_release_value(global);
    jerry_release_value(constructor);
    jerry_release_value(proto);
}

const char *get_lvgl_obj_constructor_name() {
    return NAME;
}

const jerry_object_native_info_t *get_lvgl_obj_native_info() {
    return &lvgl_obj_native_info;
}
