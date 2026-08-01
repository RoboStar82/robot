
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

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

typedef std::function<void(const uint8_t* buffer, size_t length)> script_write_callback;

typedef struct {
    const char* filename;
    const char* source;
    size_t length;
    script_write_callback write;
} script_code_t;

class Script {
   public:
    Script();
    ~Script();

    void begin();

    void end();

    void run(script_code_t code);

    void task();

    static void task(void* arg);

    script_write_callback serialWrite = [](const uint8_t* buffer, size_t length) {
        Serial.write(buffer, length);
    };

    script_write_callback write = nullptr;

   protected:
    TaskHandle_t taskStarted = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(script_code_t));

    void exec(script_code_t code);
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
JSValue js_delay(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_date_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_date_constructor(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_performance_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_performance_memory(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

#define JS_TIMERS 16

#define JS_DELAY 10

typedef struct {
    bool active;
    int64_t millis;
    JSGCRef callback;
} JSTimer;

JSValue js_setTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_clearTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

void js_timers_run(JSContext* ctx);

JSValue js_display_length(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_display_clear(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);
JSValue js_display_drawLogo(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv);

#ifdef __cplusplus
}
#endif
