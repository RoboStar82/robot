
#include "config.h"

#ifdef ROBOT_HAS_SCRIPT

#include "script.h"

Script script;

Script::Script() {}

Script::~Script() {}

void Script::begin() {
    if (!startedTask) {
        xTaskCreate(task, "script_task", 16384, NULL, 1, &startedTask);
    }
}

void Script::end() {
    if (startedTask) {
        vTaskDelete(startedTask);
        scriptEnd();
    }
}

void Script::task() {
    while (true) {
        if (Serial.available()) {
            char script[4096];
            size_t length = Serial.readBytesUntil('\n', script, sizeof(script) - 1);
            if (length) {
                script[length] = '\0';
                JSContext* ctx = scriptBegin();
                JSValue value = JS_Parse(ctx, script, length, "<input>", JS_EVAL_RETVAL | JS_EVAL_REPL);
                if (JS_IsException(value)) {
                    value = JS_GetException(ctx);
                } else {
                    value = JS_Run(ctx, value);
                    if (JS_IsException(value)) {
                        value = JS_GetException(ctx);
                    }
                }
                js_output(ctx, value);
            }
            vTaskDelay(1000);
        } else if (scriptContext) {
            js_timer(scriptContext);
            vTaskDelay(1000);
        } else {
            vTaskDelay(1000);
        }
    }
}

void Script::task(void* arg) {
    script.task();
}

void js_write(void* opaque, const void* buffer, size_t length) {
    Serial.write((const char*)buffer, length);
}

void js_output(JSContext* ctx, JSValue value) {
    JS_PrintValueF(ctx, value, JS_DUMP_LONG);
    Serial.write('\n');
}

int js_interrupt(JSContext* ctx, void* opaque) {
    static unsigned long lastMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis > 100) {
        lastMillis = currentMillis;
        js_timer(ctx);
        // Watchdog IDLE task
        vTaskDelay(1);
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
    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            Serial.write(' ');
        }
        JSValue value = argv[i];
        if (JS_IsString(ctx, value)) {
            JSCStringBuf buffer;
            const char* s;
            size_t length;
            s = JS_ToCStringLen(ctx, &length, value, &buffer);
            Serial.write(s, length);
        } else {
            JS_PrintValueF(ctx, value, JS_DUMP_LONG);
        }
    }
    Serial.write('\n');
    return JS_UNDEFINED;
}

JSValue js_performance_now(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    return JS_NewInt64(ctx, millis());
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

JSTimer js_timer_list[JS_TIMERS];

JSValue js_setTimeout(JSContext* ctx, JSValue* thisValue, int argc, JSValue* argv) {
    if (!JS_IsFunction(ctx, argv[0])) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    int delay;
    if (JS_ToInt32(ctx, &delay, argv[1])) {
        return JS_EXCEPTION;
    }
    for (int i = 0; i < JS_TIMERS; i++) {
        JSTimer* timer = &js_timer_list[i];
        if (!timer->allocated) {
            JSValue* ref = JS_AddGCRef(ctx, &timer->callback);
            *ref = argv[0];
            timer->millis = millis() + delay;
            timer->allocated = true;
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
        JSTimer* timer = &js_timer_list[i];
        if (timer->allocated) {
            JS_DeleteGCRef(ctx, &timer->callback);
            timer->allocated = false;
        }
    }
    return JS_UNDEFINED;
}

void js_timer(JSContext* ctx) {
    while (true) {
        bool isTimer = false;
        int64_t minDelay = 100;
        int64_t currentMillis = millis();
        for (int i = 0; i < JS_TIMERS; i++) {
            JSTimer* timer = &js_timer_list[i];
            if (timer->allocated) {
                int64_t delay = timer->millis - currentMillis;
                if (delay <= 0) {
                    if (JS_StackCheck(ctx, 2)) {
                        JSValue value = JS_GetException(ctx);
                        js_output(ctx, value);
                        return;
                    }
                    JS_PushArg(ctx, timer->callback.val);
                    JS_PushArg(ctx, JS_NULL);
                    JS_DeleteGCRef(ctx, &timer->callback);
                    timer->allocated = false;
                    JSValue value = JS_Call(ctx, 0);
                    if (JS_IsException(value)) {
                        value = JS_GetException(ctx);
                        js_output(ctx, value);
                    }
                } else if (delay < minDelay) {
                    minDelay = delay;
                    isTimer = true;
                }
            }
        }
        if (isTimer) {
            if (minDelay > 0) {
                vTaskDelay(minDelay);
            }
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
