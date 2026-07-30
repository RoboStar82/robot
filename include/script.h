
#pragma once

#include <Arduino.h>

#ifdef ARDUINO_STM32
#include <FreeRTOS.h>
#include <task.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <mquickjs.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#ifdef ROBOT_HAS_DISPLAY
#include "display.h"
#endif

#include "print.h"

class Script {
   public:
    Script();
    ~Script();

    void begin();

    void end();

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
};

extern Script script;

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern JSContext* scriptContext;

JSContext* scriptBegin();
void scriptEnd();

void js_write(void* opaque, const void* buffer, size_t length);
void js_output(JSContext* ctx, JSValue value);
int js_interrupt(JSContext* ctx, void* opaque);

JSValue js_gc(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_load(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_print(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_performance_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_date_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_date_constructor(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

#define JS_TIMERS 16

typedef struct {
    bool allocated;
    JSGCRef callback;
    int64_t millis;
} JSTimer;

JSValue js_setTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_clearTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

void js_timer(JSContext* ctx);

JSValue js_display_length(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_display_clear(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_display_drawLogo(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

#ifdef __cplusplus
}
#endif
