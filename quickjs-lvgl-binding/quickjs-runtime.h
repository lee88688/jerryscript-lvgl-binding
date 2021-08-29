#ifndef QUICKJS_RUNTIME_H
#define QUICKJS_RUNTIME_H

#include "quickjs/quickjs.h"

void js_runtime_loop(JSContext *ctx);
void js_runtime_init(JSContext *ctx);

#endif /* QUICKJS_RUNTIME_H */