#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "quickjs/list.h"
#include "quickjs/quickjs.h"
#include "quickjs/cutils.h"

typedef struct JSRuntimeState
{
    struct list_head os_timers;
} JSRuntimeState;

typedef struct
{
    struct list_head link;
    BOOL has_object;
    int64_t timeout;
    JSValue func;
} JSTimer;

static void unlink_timer(JSRuntime *rt, JSTimer *tm) {
    if (tm->link.prev) {
        list_del(&tm->link);
        tm->link.prev = tm->link.next = NULL;
    }
}

static void free_timer(JSRuntime *rt, JSTimer *tm) {
    JS_FreeValueRT(rt, tm->func);
    js_free_rt(rt, tm);
}

static int64_t get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

// timer class
static JSClassID js_timer_class_id;

static void js_timer_finalizer(JSRuntime *rt, JSValue val)
{
    JSTimer *tm = JS_GetOpaque(val, js_timer_class_id);
    if (tm)
    {
        tm->has_object = FALSE;
        if (!tm->link.prev)
            free_timer(rt, tm);
    }
}

static void js_timer_mark(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func) {
    JSTimer *tm = JS_GetOpaque(val, js_timer_class_id);
    if (tm)
    {
        JS_MarkValue(rt, tm->func, mark_func);
    }
}

static JSValue js_setTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    JSRuntime *rt = JS_GetRuntime(ctx);
    JSRuntimeState *rs = JS_GetRuntimeOpaque(rt);
    int64_t delay;
    JSValueConst func;
    JSTimer *tm;
    JSValue obj;

    func = argv[0];
    if (!JS_IsFunction(ctx, func))
        return JS_ThrowTypeError(ctx, "not a function");
    if (JS_ToInt64(ctx, &delay, argv[1]))
        return JS_EXCEPTION;
    obj = JS_NewObjectClass(ctx, js_timer_class_id);
    if (JS_IsException(obj))
        return obj;
    tm = js_mallocz(ctx, sizeof(*tm));
    if (!tm)
    {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    tm->has_object = TRUE;
    tm->timeout = get_time_ms() + delay;
    tm->func = JS_DupValue(ctx, func);
    list_add_tail(&tm->link, &rs->os_timers);
    JS_SetOpaque(obj, tm);
    return obj;
}

static JSValue js_clearTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    JSTimer *tm = JS_GetOpaque2(ctx, argv[0], js_timer_class_id);
    if (!tm)
        return JS_EXCEPTION;
    unlink_timer(JS_GetRuntime(ctx), tm);
    return JS_UNDEFINED;
}

static JSClassDef js_timer_class = {
    "OSTimer",
    .finalizer = js_timer_finalizer,
    .gc_mark = js_timer_mark,
};

// return -1 when an error occurs
static int call_handler(JSContext *ctx, JSValueConst func)
{
    JSValue ret, func1;
    /* 'func' might be destroyed when calling itself (if it frees the
       handler), so must take extra care */
    func1 = JS_DupValue(ctx, func);
    ret = JS_Call(ctx, func1, JS_UNDEFINED, 0, NULL);
    JS_FreeValue(ctx, func1);
    if (JS_IsException(ret)) {
        return -1;
    }
    JS_FreeValue(ctx, ret);
    return 0;
}

// return 1 when the job is executed successfully
// return 0 when no job is executed
// return -1 when job is executed with wrong
static int js_runtime_poll(JSContext *ctx) {
    JSRuntime *rt = JS_GetRuntime(ctx);
    JSRuntimeState *rs = JS_GetRuntimeOpaque(rt);
    int64_t cur_time, delay;
    struct list_head *el;
    int error_flag = 0;
    
    /* XXX: only timers and basic console input are supported */
    if (!list_empty(&rs->os_timers)) {
        cur_time = get_time_ms();
        list_for_each(el, &rs->os_timers) {
            JSTimer *tm = list_entry(el, JSTimer, link);
            delay = tm->timeout - cur_time;
            if (delay <= 0) {
                JSValue func;
                /* the timer expired */
                func = tm->func;
                tm->func = JS_UNDEFINED;
                unlink_timer(rt, tm);
                if (!tm->has_object)
                    free_timer(rt, tm);
                error_flag = call_handler(ctx, func);
                JS_FreeValue(ctx, func);
                if (error_flag == -1) return -1;
                return 1;
            }
        }
    }

    return 0;
}

// console.log
static JSValue js_lvgl_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    if (argc == 0)
        return JS_UNDEFINED;
    int i = 0;
    for (i = 0; i < argc; i++)
    {
        const char *s = JS_ToCString(ctx, argv[i]);
        printf("%s ", s);
        JS_FreeCString(ctx, s);
    }
    printf("\n");
    return JS_UNDEFINED;
}

void js_runtime_init(JSContext *ctx)
{
    JSValue global = JS_GetGlobalObject(ctx);

    // timeout function
    JS_NewClassID(&js_timer_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_timer_class_id, &js_timer_class);
    // add setTimeout and clearTimeout
    JS_SetPropertyStr(ctx, global, "setTimeout", JS_NewCFunction(ctx, js_setTimeout, "setTimeout", 2));
    JS_SetPropertyStr(ctx, global, "clearTimeout", JS_NewCFunction(ctx, js_clearTimeout, "clearTimeout", 2));

    // console
    JSValue console = JS_NewObject(ctx);
    JSValue log = JS_NewCFunction(ctx, js_lvgl_log, "log", 0);
    JS_SetPropertyStr(ctx, console, "log", JS_DupValue(ctx, log));
    JS_SetPropertyStr(ctx, console, "warn", JS_DupValue(ctx, log));
    JS_FreeValue(ctx, log);
    JS_SetPropertyStr(ctx, global, "console", console);

    JS_FreeValue(ctx, global);
}

void js_runtime_loop(JSContext *ctx) {
    JSContext *ctx1;
    int err;
    BOOL is_job_empty = FALSE;

    for(;;) {
        /* execute the pending jobs */
        for(;;) {
            err = JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1);
            if (err <= 0) {
                // no error or exit with error, assume job is empty
                if (err < 0) {
                    // todo: deal with exception
                    // js_std_dump_error(ctx1);
                }
                is_job_empty = TRUE;
                break;
            }
        }

        // only when no job and no timeout job break js running
        if (!js_runtime_poll(ctx) && is_job_empty)
            break;
    }
}
