
#pragma once

#include <driver/gpio.h>
#include <esp_rom_gpio.h>
#include <hal/gpio_ll.h>
#include <hal/gpio_types.h>

#define PLATFORM_IDENT "ESP32"

#include "../../../include/config.h"

#define SWCLK_PORT (0)

#ifndef SWCLK_PIN
#define SWCLK_PIN (ROBOT_OTA_BLACKMAGIC_SWCLK_PIN)
#endif
#ifndef SWDIO_PIN
#define SWDIO_PIN (ROBOT_OTA_BLACKMAGIC_SWDIO_PIN)
#endif

#if 0 <= SWCLK_PIN && SWCLK_PIN < 32 && 0 <= SWDIO_PIN && SWDIO_PIN < 32 && SWCLK_PIN != GPIO_NUM_MAX && SWDIO_PIN != GPIO_NUM_MAX
#define GPIO_FAST_IMPL
#endif

#ifndef TMS_PIN
#define TMS_PIN (-1)
#endif
#ifndef TDI_PIN
#define TDI_PIN (-1)
#endif
#ifndef TDO_PIN
#define TDO_PIN (-1)
#endif
#ifndef TCK_PIN
#define TCK_PIN (-1)
#endif

#ifndef TRACESWO_PIN
#define TRACESWO_PIN (-1)
#endif

#undef PLATFORM_HAS_TRACESWO

#define SWD_CYCLES_PER_CLOCK 19L
#define SWD_TOTAL_CYCLES 190L
#define SWD_DEFAULT_FREQUENCY 1000000L

#define SET_RUN_STATE(state) \
    do {                     \
    } while (0)

#define SET_IDLE_STATE(state) \
    do {                      \
    } while (0)

#define SET_ERROR_STATE(state) \
    do {                       \
    } while (0)

#define TMS_SET_MODE() \
    do {               \
    } while (0)

#define SWDIO_MODE_FLOAT()           \
    do {                             \
        platform_swdio_mode_float(); \
    } while (0)

#define SWDIO_MODE_DRIVE()           \
    do {                             \
        platform_swdio_mode_drive(); \
    } while (0)

#define gpio_set(port, pin) platform_gpio_set(pin);

#define gpio_get(port, pin) platform_gpio_get_level(pin);

#define gpio_set_val(port, pin, value) platform_gpio_set_level(pin, value);

#define gpio_clear(port, pin) platform_gpio_clear(pin);

static inline void platform_gpio_set(gpio_num_t gpio_num) {
#ifdef GPIO_FAST_IMPL
    GPIO.out_w1ts = (1 << gpio_num);
#else
    gpio_set_level((gpio_num_t)gpio_num, 1);
#endif
}

static inline int platform_gpio_get_level(gpio_num_t gpio_num) {
#ifdef GPIO_FAST_IMPL
    return (GPIO.in >> gpio_num) & 0x1;
#else
    return gpio_get_level(gpio_num);
#endif
}

static inline void platform_gpio_set_level(gpio_num_t gpio_num, uint32_t value) {
#ifdef GPIO_FAST_IMPL
    if (value) {
        GPIO.out_w1ts = (1 << gpio_num);
    } else {
        GPIO.out_w1tc = (1 << gpio_num);
    }
#else
    gpio_set_level(gpio_num, value);
#endif
}

static inline void platform_gpio_clear(gpio_num_t gpio_num) {
#ifdef GPIO_FAST_IMPL
    GPIO.out_w1tc = (1 << gpio_num);
#else
    gpio_set_level(gpio_num, 0);
#endif
}

static inline void platform_swdio_mode_float(void) {
#ifdef GPIO_FAST_IMPL
    gpio_ll_output_disable(&GPIO, SWDIO_PIN);
    gpio_ll_input_enable(&GPIO, SWDIO_PIN);
#else
    gpio_set_direction(SWDIO_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SWDIO_PIN, GPIO_FLOATING);
#endif
}

static inline void platform_swdio_mode_drive(void) {
#ifdef GPIO_FAST_IMPL
    GPIO.enable_w1ts = (0x1 << SWDIO_PIN);
    esp_rom_gpio_connect_out_signal(SWDIO_PIN, SIG_GPIO_OUT_IDX, false, false);
#else
    gpio_set_direction(SWDIO_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(SWDIO_PIN, GPIO_FLOATING);
#endif
}

void platform_init();
void platform_loop();
void platform_main();

void platform_printf(const char* format, ...);
