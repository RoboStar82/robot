
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <mquickjs.h>
#ifdef __cplusplus
}
#endif

#include "config.h"

#ifdef __cplusplus

#include <string>

#include "print.h"

typedef struct {
    const char* source;
    size_t length;
    const char* filename;
    bool* finished;
    bool buffering;
} ScriptCode_t;

class Script {
   public:
    Script();
    virtual ~Script() = default;

    void begin();
    void end();

    void run(const char* source, size_t length, std::string& output, const char* filename = nullptr);
    void run(const char* source, size_t length, const char* filename = nullptr, bool wait = true);

    void write(const char* buffer, size_t length);

    void task();

   protected:
    TaskHandle_t taskHandle = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(ScriptCode_t));

    std::string outputBuffer;
    bool outputBuffering = false;

    void exec(const char* source, size_t length, const char* filename, bool* finished, bool buffering);

    static inline void task(void* arg);
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
    unsigned long start;
    unsigned int wait;
    JSGCRef callback;
    bool started;
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
