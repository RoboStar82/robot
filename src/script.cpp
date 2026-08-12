
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <string>

#include "config.h"

#ifdef ROBOT_HAS_SCRIPT

#include "script.h"

#ifdef ROBOT_HAS_DISPLAY
#include "display.h"
#endif

#include "delay.h"
#include "print.h"

Script script;

Script::Script() {}

Script::~Script() {}

void Script::begin() {
    if (!taskStarted) {
        xTaskCreate(task, "script_task", 16384, NULL, 1, &taskStarted);
    }
}

void Script::end() {
    if (taskStarted) {
        vTaskDelete(taskStarted);
        taskStarted = nullptr;
        scriptEnd();
    }
}

void Script::run(ScriptCode_t code) {
    xQueueSendMS(taskQueue, &code, 0);
}

void Script::exec(ScriptCode_t code) {
    if (code.length > 0 && code.source[code.length - 1] == '\n') {
        print("[script] %s\n%s<END>\n", code.filename, code.source);
    } else {
        print("[script] %s\n%s\n<END>\n", code.filename, code.source);
    }
    JSContext* ctx = scriptBegin();
    write = code.write;
    JSValue value = JS_Parse(ctx, code.source, code.length, code.filename, JS_EVAL_RETVAL);
    if (JS_IsException(value)) {
        value = JS_GetException(ctx);
    } else {
        value = JS_Run(ctx, value);
        if (JS_IsException(value)) {
            value = JS_GetException(ctx);
        }
    }
    print("[result] ");
    js_output(ctx, value);
    js_timers_run(ctx);
    if (write) {
        write(nullptr, 0);
        write = nullptr;
    }
}

void Script::task() {
    std::string source;
    size_t length = 0;
    unsigned long start = 0;
    ScriptCode_t code;
    while (true) {
        if (RobotSerial.available() > 0) {
            start = millis();
            source.resize(length + RobotSerial.available());
            while (RobotSerial.available() > 0) {
                if (length >= source.length()) {
                    source.resize(length + RobotSerial.available());
                }
                source[length] = RobotSerial.read();
                length++;
            }
            if (length > 1 && source[length - 1] == '\n') {
                exec({
                    .filename = "<Serial>",
                    .source = source.c_str(),
                    .length = length,
                    .write = nullptr,
                });
                length = 0;
            } else {
                js_timers_run(scriptContext);
            }
        } else if (xQueueReceiveMS(taskQueue, &code, length ? JS_DELAY : 1000)) {
            exec(code);
        } else if (length) {
            if (millis() - start >= 1000) {
                exec({
                    .filename = "<Serial>",
                    .source = source.c_str(),
                    .length = length,
                    .write = nullptr,
                });
                length = 0;
            } else {
                js_timers_run(scriptContext);
            }
        } else if (scriptContext) {
            js_timers_run(scriptContext);
        } else {
            vTaskDelayMS(1000);
        }
    }
}

void Script::task(void* arg) {
    script.task();
}

void js_write(void* opaque, const void* buffer, size_t length) {
    if (script.write) {
        script.write((const uint8_t*)buffer, length);
    }
    script.serialWrite((const uint8_t*)buffer, length);
}

void js_output(JSContext* ctx, JSValue value) {
    JS_PrintValueF(ctx, value, JS_DUMP_LONG);
    js_write(nullptr, "\n", 1);
}

int js_interrupt(JSContext* ctx, void* opaque) {
    static unsigned long start = 0;
    unsigned long now = millis();
    if (now - start > JS_DELAY) {
        start = now;
        js_timers_run(ctx);
    }
    return 0;
}

JSValue js_gc(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    JS_GC(ctx);
    return JS_UNDEFINED;
}

JSValue js_load(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    return JS_UNDEFINED;
}

JSValue js_print(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    print("[console] ");
    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            js_write(nullptr, " ", 1);
        }
        JSValue value = argv[i];
        if (JS_IsString(ctx, value)) {
            JSCStringBuf buffer;
            const char* s;
            size_t length;
            s = JS_ToCStringLen(ctx, &length, value, &buffer);
            js_write(nullptr, s, length);
        } else {
            JS_PrintValueF(ctx, value, JS_DUMP_LONG);
        }
    }
    js_write(nullptr, "\n", 1);
    return JS_UNDEFINED;
}

JSValue js_delay(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    int wait;
    if (JS_ToInt32(ctx, &wait, argv[0])) {
        return JS_EXCEPTION;
    }
    if (wait <= 1) {
        js_timers_run(ctx);
    } else if (wait <= JS_DELAY) {
        vTaskDelayMS(wait - 1);
        js_timers_run(ctx);
    } else {
        unsigned long start = millis();
        vTaskDelayMS(JS_DELAY);
        js_timers_run(ctx);
        while (true) {
            unsigned long now = millis();
            if (now - start >= wait) {
                break;
            } else {
                int ms = start + wait - now;
                if (ms > JS_DELAY) {
                    vTaskDelayMS(JS_DELAY);
                    js_timers_run(ctx);
                } else if (ms > 0) {
                    vTaskDelayMS(ms);
                    break;
                }
            }
        }
    }
    return JS_UNDEFINED;
}

JSValue js_date_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    return JS_NewInt64(ctx, millis());
}

JSValue js_date_constructor(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    argc &= ~FRAME_CF_CTOR;
    double value;
    if (argc == 0) {
        value = millis();
    } else if (argc == 1 && JS_IsNumber(ctx, argv[0])) {
        if (JS_ToNumber(ctx, &value, argv[0])) {
            return JS_EXCEPTION;
        }
    } else {
        return JS_ThrowTypeError(ctx, "invalid date");
    }
    return JS_NewDate(ctx, value);
}

JSValue js_performance_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    return JS_NewInt64(ctx, millis());
}

JSValue js_performance_memory(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    JSValue obj = JS_NewObject(ctx);
    if (JS_IsException(obj)) {
        return obj;
    }
    char** ptr = (char**)ctx;
    JSValue e;
    e = JS_SetPropertyStr(ctx, obj, "jsHeapSizeLimit", JS_NewInt32(ctx, ptr[2] - ptr[0] - 512));
    if (JS_IsException(e)) {
        return e;
    }
    e = JS_SetPropertyStr(ctx, obj, "usedJSHeapSize", JS_NewInt32(ctx, ptr[1] - ptr[0]));
    if (JS_IsException(e)) {
        return e;
    }
    e = JS_SetPropertyStr(ctx, obj, "totalJSHeapSize", JS_NewInt32(ctx, ptr[1] - ptr[0]));
    if (JS_IsException(e)) {
        return e;
    }
    return obj;
}

JSTimer js_timers[JS_TIMERS];

JSValue js_setTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    if (!JS_IsFunction(ctx, argv[0])) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    int wait;
    if (JS_ToInt32(ctx, &wait, argv[1])) {
        return JS_EXCEPTION;
    }
    for (int i = 0; i < JS_TIMERS; i++) {
        JSTimer* timer = &js_timers[i];
        if (!timer->started) {
            JSValue* ref = JS_AddGCRef(ctx, &timer->callback);
            *ref = argv[0];
            timer->start = millis();
            timer->wait = wait > 0 ? wait : 0;
            timer->started = true;
            return JS_NewInt32(ctx, i);
        }
    }
    return JS_ThrowInternalError(ctx, "too many timers");
}

JSValue js_clearTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    int i;
    if (JS_ToInt32(ctx, &i, argv[0])) {
        return JS_EXCEPTION;
    }
    if (i >= 0 && i < JS_TIMERS) {
        JSTimer* timer = &js_timers[i];
        if (timer->started) {
            JS_DeleteGCRef(ctx, &timer->callback);
            timer->started = false;
        }
    }
    return JS_UNDEFINED;
}

void js_timers_run(JSContext* ctx) {
    vTaskDelayMS(1);
    while (true) {
        bool started = false;
        int wait = JS_DELAY;
        unsigned long now = millis();
        for (int i = 0; i < JS_TIMERS; i++) {
            JSTimer* timer = &js_timers[i];
            if (timer->started) {
                if (now - timer->start >= timer->wait) {
                    if (JS_StackCheck(ctx, 2)) {
                        JSValue value = JS_GetException(ctx);
                        js_output(ctx, value);
                        return;
                    }
                    JS_PushArg(ctx, timer->callback.val);
                    JS_PushArg(ctx, JS_NULL);
                    JS_DeleteGCRef(ctx, &timer->callback);
                    timer->started = false;
                    JSValue value = JS_Call(ctx, 0);
                    if (JS_IsException(value)) {
                        value = JS_GetException(ctx);
                        js_output(ctx, value);
                    }
                } else {
                    int ms = timer->start + timer->wait - now;
                    if (0 < ms && ms < wait) {
                        started = true;
                        wait = ms;
                    }
                }
            }
        }
        if (started) {
            vTaskDelayMS(wait);
        } else {
            break;
        }
    }
}

JSValue js_display_length(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
#ifdef ROBOT_HAS_DISPLAY
    return JS_NewInt64(ctx, display.length());
#endif
    return JS_NewInt64(ctx, 0);
}

JSValue js_display_clear(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
#ifdef ROBOT_HAS_DISPLAY
    display.clear();
#endif
    return JS_UNDEFINED;
}

JSValue js_display_drawLogo(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
#ifdef ROBOT_HAS_DISPLAY
    display.drawLogo();
#endif
    return JS_UNDEFINED;
}

#endif
