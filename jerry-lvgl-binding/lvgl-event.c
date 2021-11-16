#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"
#include "lvgl-event.h"

static const char *NAME = "_eProto";

/**
 * fixme: this may cause compatible problems.
 * the code is just copy of lv_event.c
 * because lvgl does not provide more api about lv_event_dsc_t
 */
typedef struct _lv_event_dsc_t {
    lv_event_cb_t cb;
    void * user_data;
    lv_event_code_t filter : 8;
} lv_event_dsc_t;


static void lvgl_event_free_cb (void *native_p, jerry_object_native_info_t *info_p) {
    BI_LOG_TRACE("event style");
    lv_event_t *e = (lv_event_t *) native_p;
    free(e);
}

static const jerry_object_native_info_t lvgl_event_native_info = {
    .free_cb = lvgl_event_free_cb,
};

jerry_value_t lvgl_event_create(lv_event_t *event) {
    jerry_value_t e = jerry_create_object();
    lv_event_t *event_copy = (lv_event_t *) malloc(sizeof(lv_event_t));
    memcpy(event_copy, event, sizeof(lv_event_t));
    jerry_set_object_native_pointer(
        e,
        event_copy,
        &lvgl_event_native_info
    );

    jerry_value_t global = jerry_get_global_object();
    jerry_value_t proto = jerryx_get_property_str(global, NAME);

    jerryx_set_property_str(e, "prototype", proto);

    jerry_release_value(proto);
    jerry_release_value(global);

    return e;
}

/**
 * original event will be freed after event cb.
 * some data may be invalid and should be clear.
 */
void lvgl_event_clear_after_call(jerry_value_t e) {
    lv_event_t *event = NULL;
    jerry_get_object_native_pointer(e, (void **)&event, &lvgl_event_native_info);
    if (event) {
        event->prev = NULL;
        // clear target, use user data target instead
        // todo: how to support event bubbling(current target)
        event->target = NULL;
        event->current_target = NULL;
        // param set to NULL
        event->param = NULL;
    }
}

lvgl_event_user_data_t *lvgl_event_get_user_data_from_obj(lv_obj_t *obj, jerry_value_t func) {
    uint32_t i;
    for(i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        lvgl_event_user_data_t *user_data = (lvgl_event_user_data_t *) obj->spec_attr->event_dsc[i].user_data;
        if (user_data && user_data->func == func) {
            return user_data;
        }
    }
    return NULL;
}

// clear event cb user data before obj is freed
void lvgl_event_clear_user_data(lv_obj_t *obj) {
    uint32_t i;
    for(i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        lvgl_event_user_data_t *user_data = obj->spec_attr->event_dsc[i].user_data;
        jerry_release_value(user_data->func);
        jerry_release_value(user_data->target);
        free(user_data);
        obj->spec_attr->event_dsc[i].user_data = NULL;
    }
}

static jerry_value_t lvgl_event_get_code(const jerry_call_info_t *info, const jerry_value_t argv[], const jerry_length_t argc) {
    lv_event_t *event = NULL;
    if (jerry_get_object_native_pointer(info->this_value, (void **) &event, &lvgl_event_native_info)) {
        lv_event_code_t code = lv_event_get_code(event);
        return jerry_create_number(code);
    }
    return jerry_create_undefined();
}

static const jerry_function_entry js_event_prototype_methods[] = {
    JERRY_CFUNC_DEF("getCode", lvgl_event_get_code),
};

void js_lvgl_event_init() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t proto = jerry_create_object();

    jerryx_set_property_str(global, NAME, proto);
    jerry_set_prop_list(proto, js_event_prototype_methods, countof(js_event_prototype_methods));

    jerry_release_value(proto);
    jerry_release_value(global);
}