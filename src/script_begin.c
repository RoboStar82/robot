
#include "config.h"

#ifdef ROBOT_HAS_SCRIPT

#include "script.h"
#include "script_begin.h"

JSContext* scriptContext = NULL;

uint8_t* scriptMemory = NULL;

JSContext* scriptBegin() {
    if (!scriptContext) {
        if (scriptMemory = malloc(16384)) {
            scriptContext = JS_NewContext(scriptMemory, 16384, &js_stdlib);
            JS_SetInterruptHandler(scriptContext, js_interrupt);
            JS_SetLogFunc(scriptContext, js_write);
        }
    }
    return scriptContext;
}

void scriptEnd() {
    if (scriptContext) {
        JS_FreeContext(scriptContext);
    }
    if (scriptMemory) {
        free(scriptMemory);
    }
}

#endif
