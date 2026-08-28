
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_PYTHON

#ifdef __cplusplus
extern "C" {
#endif
#include <py/builtin.h>
#include <py/compile.h>
#include <py/gc.h>
#include <py/lexer.h>
#include <py/parse.h>
#include <py/runtime.h>
#include <py/stackctrl.h>
void mp_hal_stdout_tx_strn_cooked(const char* str, size_t len);
const mp_obj_fun_builtin_var_t mp_builtin_open_obj = {0};
#ifdef __cplusplus
}
#endif

#include "delay.h"
#include "print.h"
#include "python.h"

void mp_hal_stdout_tx_strn_cooked(const char* str, size_t len) {
    print((const uint8_t*)str, len);
}

mp_lexer_t* mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(1);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

Python python;

Python::Python() {}

Python::~Python() {}

char heap[8192];

const char* src = "print(1+1)";

void Python::begin() {
    int stack_top;
    mp_stack_set_top(&stack_top);
    gc_init(heap, heap + sizeof(heap));
    mp_init();

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Compile, parse and execute the given string.
        mp_lexer_t* lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }

    mp_deinit();
}

void Python::end() {
    mp_deinit();
}

void Python::task() {}

void Python::task(void* arg) {
    python.task();
}

#endif
