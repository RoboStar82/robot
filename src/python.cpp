
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <string>

#include "config.h"

#ifdef ROBOT_HAS_PYTHON

#include "python.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <py/gc.h>
#include <py/lexer.h>
#include <py/parse.h>
#include <py/compile.h>
#include <py/runtime.h>
#include <py/builtin.h>
#include <py/stackctrl.h>
void mp_hal_stdout_tx_strn_cooked(const char* buffer, size_t length);
#ifdef __cplusplus
}
#endif

#include "delay.h"
#include "print.h"

Python python;

Python::Python() {}

void Python::begin() {
    if (!taskHandle) {
        xTaskCreate(task, "python_task", 16384, NULL, 1, &taskHandle);
    }
}

void Python::end() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
        if (initCalled) {
            mp_deinit();
        }
    }
}

void Python::run(const char* source, size_t length, std::string& output, const char* filename) {
    bool finished = false;
    PythonCode_t code = {
        .source = source,
        .length = length,
        .filename = filename,
        .finished = &finished,
        .buffering = true,
    };
    xQueueSendMS(taskQueue, &code, 0);
    vTaskDelayMS(1);
    while (!finished) {
        vTaskDelayMS(1);
    }
    output = outputBuffer;
}

void Python::run(const char* source, size_t length, const char* filename, bool wait) {
    bool finished = false;
    PythonCode_t code = {
        .source = source,
        .length = length,
        .filename = filename,
        .finished = wait ? &finished : nullptr,
        .buffering = false,
    };
    xQueueSendMS(taskQueue, &code, 0);
    if (wait) {
        vTaskDelayMS(1);
        while (!finished) {
            vTaskDelayMS(1);
        }
    }
}

void Python::write(const char* buffer, size_t length) {
    if (!outputBuffering) {
        return;
    }
    outputBuffer.append(buffer, length);
}

void Python::exec(const char* source, size_t length, const char* filename, bool* finished, bool buffering) {
    if (!filename) {
        filename = "<input>";
    }
    print("[python] %s\n", filename);
    print((const uint8_t*)source, length);
    print(length > 1 && source[length - 1] == '\n' ? "<END>\n" : "\n<END>\n");
    outputBuffering = buffering;
    outputBuffer.resize(0);
    if (!initCalled) {
        initCalled = true;
        int stackTop;
        mp_stack_set_top(&stackTop);
        gc_init(heap, heap + sizeof(heap));
        mp_init();
    }
    nlr_buf_t nlr;
    if (!nlr_push(&nlr)) {
        qstr qFilename = qstr_from_str(filename);
        mp_lexer_t* lexer = mp_lexer_new_from_str_len(qFilename, source, length, 0);
        mp_parse_tree_t parsed = mp_parse(lexer, MP_PARSE_FILE_INPUT);
        mp_obj_t module = mp_compile(&parsed, qFilename, true);
        mp_call_function_0(module);
        nlr_pop();
    } else {
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
    print("[result]");
    outputBuffering = false;
    if (finished != nullptr) {
        *finished = true;
    }
    if (buffering) {
        vTaskDelayMS(1);
    }
}

void Python::task() {
    PythonCode_t code;
    while (true) {
        if (xQueueReceiveMS(taskQueue, &code, 1000) == pdTRUE) {
            exec(code.source, code.length, code.filename, code.finished, code.buffering);
        }
    }
}

void Python::task(void* arg) {
    python.task();
}

const mp_obj_fun_builtin_var_t mp_builtin_open_obj = {0};

void mp_hal_stdout_tx_strn_cooked(const char* buffer, size_t length) {
    print((const uint8_t*)buffer, length);
}

mp_lexer_t* mp_lexer_new_from_file(qstr filename) {
    return nullptr;
}

mp_import_stat_t mp_import_stat(const char* path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

#endif
