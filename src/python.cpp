
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_PYTHON

#ifdef __cplusplus
extern "C" {
#endif
#include <py/gc.h>
#include <py/runtime.h>
#ifdef __cplusplus
}
#endif

#include "delay.h"
#include "print.h"
#include "python.h"

Python python;

Python::Python() {}

Python::~Python() {}

void Python::begin() {
    mp_init();
}

void Python::end() {}

void Python::task() {}

void Python::task(void* arg) {}

#endif
