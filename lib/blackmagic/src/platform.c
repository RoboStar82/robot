
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
    DEBUG_INFO("[GDB] platform_target_voltage(): 3.3v\n");
    return "3.3v";
}

int platform_hwversion(void) {
    DEBUG_INFO("[GDB] platform_hwversion(): 0\n");
    return 0;
}

bool nrst_value = false;

void platform_nrst_set_val(bool value) {
    DEBUG_INFO("[GDB] platform_nrst_set_val(%d)\n", value);
    if (value) {
        gpio_set_direction(NRST_PIN, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(NRST_PIN, GPIO_FLOATING);
        gpio_set_level(NRST_PIN, 0);
    } else {
        gpio_set_direction(NRST_PIN, GPIO_MODE_DEF_DISABLE);
    }
    nrst_value = value;
}

bool platform_nrst_get_val() {
    DEBUG_INFO("[GDB] platform_nrst_get_val(): %d\n", nrst_value);
    return nrst_value;
}

bool platform_target_get_power(void) {
    DEBUG_INFO("[GDB] platform_target_get_power(): 0\n");
    return false;
}

bool platform_target_set_power(bool power) {
    DEBUG_INFO("[GDB] platform_target_set_power(%d): 0\n", power);
    return false;
}

void platform_request_boot(void) {
    DEBUG_INFO("[GDB] platform_request_boot()\n");
    return;
}

uint32_t target_clk_divider = 0;

void platform_max_frequency_set(uint32_t frequency) {
    if (frequency < 50000) {
        DEBUG_INFO("[GDB] platform_max_frequency_set(%d)\n", frequency);
        return;
    }
    int32_t count = (esp_clk_cpu_freq() - SWD_TOTAL_CYCLES * (int32_t)frequency) / (SWD_CYCLES_PER_CLOCK * (int32_t)frequency);
    target_clk_divider = count > 0 ? count : 0;
    DEBUG_INFO("[GDB] platform_max_frequency_set(%d): %d\n", frequency, target_clk_divider);
}

uint32_t platform_max_frequency_get(void) {
    uint32_t r = esp_clk_cpu_freq() / (target_clk_divider * SWD_CYCLES_PER_CLOCK + SWD_TOTAL_CYCLES);
    DEBUG_INFO("[GDB] platform_max_frequency_get(): %d\n", r);
    return r;
}

void platform_target_clk_output_enable(bool enable) {
    DEBUG_INFO("[GDB] platform_target_clk_output_enable(): %d\n", enable);
}

void platform_ospeed_update(uint32_t frequency) {
    DEBUG_INFO("[GDB] platform_ospeed_update(): %d\n", frequency);
}

uint32_t platform_time_ms(void) {
    return esp_timer_get_time() / 1000;
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
    DEBUG_INFO("[GDB] platform_spi_chip_select(%d)\n", device_select);
    return false;
}

uint8_t platform_spi_xfer(spi_bus_e bus, uint8_t value) {
    DEBUG_INFO("[GDB] platform_spi_xfer(0x%02x)\n", value);
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
        if (c == '\3' || c == '\4') {
            target_halt_request(cur_target);
        }
        platform_pace_poll();
    }
    SET_IDLE_STATE(true);
    const gdb_packet_s* const packet = gdb_packet_receive();
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
            DEBUG_INFO("[GDB] gdb_main(\"%s\")\n", packet->data);
        } else if (packet->size > 1) {
            DEBUG_INFO("[GDB] gdb_main(%d 0x%02x...)\n", packet->size, packet->data[0]);
        } else {
            DEBUG_INFO("[GDB] gdb_main(0x%02x)\n", packet->data[0]);
        }
    }
	if (packet->data[0] != '\4' || cur_target) {
        SET_IDLE_STATE(false);
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
