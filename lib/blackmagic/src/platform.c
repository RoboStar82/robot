
#include "platform.h"

#include <FreeRTOS.h>
#include <driver/gpio.h>
#include <esp_private/esp_clk.h>
#include <exception.h>
#include <gdb_if.h>
#include <gdb_main.h>
#include <gdb_packet.h>
#include <general.h>
#include <jtagtap.h>
#include <platform_support.h>
#include <spi_types.h>
#include <swd.h>
#include <timing.h>

const char* platform_target_voltage(void) {
    return "3.3v";
}

int platform_hwversion(void) {
    return 0;
}

void platform_nrst_set_val(bool assert) {
    return;
}

bool platform_nrst_get_val() {
    return false;
}

bool platform_target_get_power(void) {
    return false;
}

bool platform_target_set_power(bool power) {
    return false;
}

void platform_request_boot(void) {
    return;
}

uint32_t target_clk_divider = 0;

void platform_max_frequency_set(uint32_t frequency) {
    if (frequency < 50000) return;
    int32_t count = (esp_clk_cpu_freq() - SWD_TOTAL_CYCLES * (int32_t)frequency) / (SWD_CYCLES_PER_CLOCK * (int32_t)frequency);
    target_clk_divider = count > 0 ? count : 0;
}

uint32_t platform_max_frequency_get(void) {
    return esp_clk_cpu_freq() / (target_clk_divider * SWD_CYCLES_PER_CLOCK + SWD_TOTAL_CYCLES);
}

void platform_target_clk_output_enable(bool enable) {
    return;
}

void platform_ospeed_update(uint32_t frequency) {
    return;
}

uint32_t platform_time_ms(void) {
    return esp_timer_get_time() / 1000;
}

void platform_delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void platform_timeout_set(platform_timeout_s* t, uint32_t ms) {
    t->time = platform_time_ms() + ms;
}

bool platform_timeout_is_expired(const platform_timeout_s* t) {
    return platform_time_ms() > t->time;
}

jtag_proc_s jtag_proc;

void jtagtap_init(void) {
    DEBUG_INFO("[GDB] jtagtap_init()\n");
}

bool platform_spi_init(spi_bus_e bus) {
    DEBUG_INFO("[GDB] platform_spi_init()\n");
    return false;
}

bool platform_spi_deinit(spi_bus_e bus) {
    DEBUG_INFO("[GDB] platform_spi_deinit()\n");
    return false;
}

bool platform_spi_chip_select(uint8_t device_select) {
    DEBUG_INFO("[GDB] platform_spi_chip_select()\n");
    return false;
}

uint8_t platform_spi_xfer(spi_bus_e bus, uint8_t value) {
    DEBUG_INFO("[GDB] platform_spi_xfer()\n");
    return 0;
}

void platform_init() {
    DEBUG_INFO("[GDB] platform_init()\n");
#ifdef GPIO_FAST_IMPL
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1 << SWCLK_PIN) | (1 << SWDIO_PIN));
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
#else
    gpio_reset_pin(SWCLK_PIN);
    gpio_reset_pin(SWDIO_PIN);
    gpio_intr_disable(SWCLK_PIN);
    gpio_intr_disable(SWDIO_PIN);
    gpio_set_direction(SWCLK_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SWDIO_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(SWCLK_PIN, GPIO_FLOATING);
    gpio_set_pull_mode(SWDIO_PIN, GPIO_FLOATING);
#endif
    platform_max_frequency_set(SWD_DEFAULT_FREQUENCY);
}

void platform_loop() {
    SET_IDLE_STATE(false);
    while (gdb_target_running && cur_target) {
        gdb_poll_target();
        if (!gdb_target_running || !cur_target) {
            break;
        }
        char c = gdb_if_getchar_to(0);
        if (c == '\x03' || c == '\x04') {
            target_halt_request(cur_target);
        }
        platform_pace_poll();
    }
    SET_IDLE_STATE(true);
    const gdb_packet_s* const packet = gdb_packet_receive();
    if (packet->data[0] != '\x04' || cur_target) {
        SET_IDLE_STATE(false);
    }
    if (!packet->size) {
        DEBUG_INFO("[GDB] gdb_main()\n");
    } else {
        bool isPrint = true;
        for (size_t i = 0; i < packet->size; i++) {
            char c = packet->data[i];
            if (!(0x20 <= c && c < 0x7f)) {
                isPrint = false;
                break;
            }
        }
        if (isPrint) {
            DEBUG_INFO("[GDB] gdb_main(%s)\n", packet->data);
        } else if (packet->size > 1) {
            DEBUG_INFO("[GDB] gdb_main(%d 0x%02x...)\n", packet->size, packet->data[0]);
        } else {
            DEBUG_INFO("[GDB] gdb_main(0x%02x)\n", packet->data[0]);
        }
    }
    gdb_main(packet);
}

void platform_main() {
    platform_init();
    while (true) {
        TRY(EXCEPTION_ALL) {
            platform_loop();
        }
        CATCH() {
            default:
                gdb_put_packet_error(0xffU);
                gdb_outf("Uncaught exception: %s\n", exception_frame.msg);
                DEBUG_ERROR("[GDB] Uncaught exception: %s\n", exception_frame.msg);
                target_list_free();
        }
    }
}
