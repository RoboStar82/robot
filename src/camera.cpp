
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_CAMERA

#include <HardwareTimer.h>
#include <Wire.h>

#include "camera.h"

#ifdef ROBOT_HAS_DISPLAY
#include "display.h"
#endif

#include "delay.h"
#include "print.h"

Camera camera;

#ifdef ROBOT_HAS_CAMERA_OV2640

#include "driver_ov2640_interface.h"

ov2640_handle_t cameraOV2640 = {
    .sccb_init = ov2640_interface_sccb_init,
    .sccb_deinit = ov2640_interface_sccb_deinit,
    .sccb_read = ov2640_interface_sccb_read,
    .sccb_write = ov2640_interface_sccb_write,
    .power_down_init = ov2640_interface_power_down_init,
    .power_down_deinit = ov2640_interface_power_down_deinit,
    .power_down_write = ov2640_interface_power_down_write,
    .reset_init = ov2640_interface_reset_init,
    .reset_deinit = ov2640_interface_reset_deinit,
    .reset_write = ov2640_interface_reset_write,
    .delay_ms = ov2640_interface_delay_ms,
    .debug_print = ov2640_interface_debug_print,
};

int cameraOV2640FPS = 0;

TwoWire cameraOV2640Wire;
HardwareTimer cameraOV2640Timer;
DMA_HandleTypeDef cameraOV2640DMA;
DCMI_HandleTypeDef cameraOV2640DCMI;

__attribute__((aligned(32)))
uint16_t cameraOV2640FrameBuffer[CAMERA_OV2640_IMAGE_WIDTH * CAMERA_OV2640_IMAGE_HEIGHT] = {0};

#ifdef __cplusplus
extern "C" {
#endif
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hDCMI);
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef* hDCMI);
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef* hDCMI);
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef* hDCMI);
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hDCMI);
void DMA1_Stream0_IRQHandler();
void DCMI_IRQHandler();
#ifdef __cplusplus
}
#endif

#endif

Camera::Camera() {}

void Camera::begin() {
    if (!taskHandle) {
        xTaskCreate(task, "camera_task", 4096, NULL, 1, &taskHandle);
    }
}

void Camera::init() {
    memset(cameraOV2640FrameBuffer, 0, sizeof(cameraOV2640FrameBuffer));

    // DMA

    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

    // DCMI

    cameraOV2640DCMI.Instance = DCMI;
    cameraOV2640DCMI.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    cameraOV2640DCMI.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
    cameraOV2640DCMI.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    cameraOV2640DCMI.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    cameraOV2640DCMI.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    cameraOV2640DCMI.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    cameraOV2640DCMI.Init.JPEGMode = DCMI_JPEG_DISABLE;
    cameraOV2640DCMI.Init.ByteSelectMode = DCMI_BSM_ALL;
    cameraOV2640DCMI.Init.ByteSelectStart = DCMI_OEBS_ODD;
    cameraOV2640DCMI.Init.LineSelectMode = DCMI_LSM_ALL;
    cameraOV2640DCMI.Init.LineSelectStart = DCMI_OELS_ODD;

    HAL_DCMI_Init(&cameraOV2640DCMI);

    // SCCB

    uint8_t r = 0;
    if (r = ov2640_init(&cameraOV2640)) {
        print("[camera] ov2640_init: %d\n", r);
    }
    if (r = ov2640_table_init(&cameraOV2640)) {
        print("[camera] ov2640_table_init: %d\n", r);
    }

#ifdef CAMERA_OV2640_YUV422
    r = ov2640_set_dvp_output_format(&cameraOV2640, OV2640_DVP_OUTPUT_FORMAT_YUV422);
    r = ov2640_set_yuv422(&cameraOV2640, OV2640_BOOL_TRUE);
#else
    r = ov2640_table_rgb565_init(&cameraOV2640)) {
#endif

    r = ov2640_set_resolution(&cameraOV2640, OV2640_RESOLUTION_SVGA);
    r = ov2640_set_image_horizontal(&cameraOV2640, 800);
    r = ov2640_set_image_vertical(&cameraOV2640, 600);
    r = ov2640_set_horizontal_size(&cameraOV2640, 800 >> 2);
    r = ov2640_set_vertical_size(&cameraOV2640, 600 >> 2);

    r = ov2640_set_output_width(&cameraOV2640, CAMERA_OV2640_IMAGE_WIDTH >> 2);
    r = ov2640_set_output_height(&cameraOV2640, CAMERA_OV2640_IMAGE_HEIGHT >> 2);
    r = ov2640_set_offset_x(&cameraOV2640, 0);
    r = ov2640_set_offset_y(&cameraOV2640, 0);

    r = ov2640_set_agc_control(&cameraOV2640, OV2640_CONTROL_AUTO);
    r = ov2640_set_agc_gain_ceiling(&cameraOV2640, OV2640_AGC_16X);
    r = ov2640_set_exposure_control(&cameraOV2640, OV2640_CONTROL_AUTO);
    r = ov2640_set_band_filter(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_band(&cameraOV2640, OV2640_BAND_50HZ);

#if false
    r = ov2640_set_horizontal_mirror(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_vertical_flip(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_mode(&cameraOV2640, OV2640_MODE_NORMAL);
    r = ov2640_set_clock_rate_double(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_clock_divider(&cameraOV2640, 0x00);
    r = ov2640_set_power_reset_pin_remap(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_output_drive(&cameraOV2640, OV2640_OUTPUT_DRIVE_2_CAPABILITY);
    r = ov2640_set_zoom_window_horizontal_start_point(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_clock_output_power_down_pin_status(&cameraOV2640, OV2640_PIN_STATUS_TRI_STATE);
    r = ov2640_set_zoom_mode_vertical_window_start_point(&cameraOV2640, 0x0000);
    r = ov2640_set_luminance_signal_high_range(&cameraOV2640, 0x40);
    r = ov2640_set_luminance_signal_low_range(&cameraOV2640, 0x38);
    r = ov2640_set_fast_mode_large_step_range(&cameraOV2640, 0x8, 0x2);
    r = ov2640_set_frame_length_adjustment(&cameraOV2640, 0x0000);
    r = ov2640_set_auto_band(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_live_video_after_snapshot(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_histogram_algorithm_low_level(&cameraOV2640, 0x70);
    r = ov2640_set_histogram_algorithm_high_level(&cameraOV2640, 0x80);
    r = ov2640_set_50hz_banding_aec(&cameraOV2640, 0x0CA);
    r = ov2640_set_60hz_banding_aec(&cameraOV2640, 0x0A8);
    r = ov2640_set_zoom(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_color_bar_test(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_pclk(&cameraOV2640, OV2640_PCLK_NO_EFFECT);
    r = ov2640_set_horizontal_window_start(&cameraOV2640, 142);
    r = ov2640_set_horizontal_window_end(&cameraOV2640, 942);
    r = ov2640_set_vertical_window_line_start(&cameraOV2640, 7);
    r = ov2640_set_vertical_window_line_end(&cameraOV2640, 607);
    r = ov2640_set_vsync_pulse_width(&cameraOV2640, 0x0000);
    r = ov2640_set_agc_gain(&cameraOV2640, 0x00ff);
    r = ov2640_set_dummy_frame(&cameraOV2640, OV2640_DUMMY_FRAME_NONE);
    r = ov2640_set_aec(&cameraOV2640, 0x0CC);
    r = ov2640_set_frame_exposure_pre_charge_row_number(&cameraOV2640, 0x40);
    r = ov2640_set_chsync_href_swap(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_href_chsync_swap(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_pclk_output_qualified_by_href(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_pclk_edge(&cameraOV2640, OV2640_EDGE_FALLING);
    r = ov2640_set_href_polarity(&cameraOV2640, OV2640_POLARITY_POSITIVE);
    r = ov2640_set_vsync_polarity(&cameraOV2640, OV2640_POLARITY_POSITIVE);
    r = ov2640_set_hsync_polarity(&cameraOV2640, OV2640_POLARITY_POSITIVE);
    r = ov2640_set_line_interval_adjust(&cameraOV2640, 0x000);
    r = ov2640_set_hsync_position_and_width_start_point(&cameraOV2640, 0x08);
    r = ov2640_set_hsync_position_and_width_end_point(&cameraOV2640, 0x30);
    r = ov2640_set_luminance_average(&cameraOV2640, 0x00);
    r = ov2640_set_flash_light(&cameraOV2640, 0x20);
    r = ov2640_set_16_zone_average_weight_option(&cameraOV2640, 0x00000000);
    r = ov2640_set_dsp_bypass(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_bpc(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_wpc(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_dvp_pclk(&cameraOV2640, 0x02);
    r = ov2640_set_cip(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_dmy(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_raw_gma(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_dg(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_awb(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_awb_gain(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_pre(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_dvp_y8(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_jpeg_output(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_dvp_jpeg_output_href_timing(&cameraOV2640, OV2640_HREF_TIMING_SENSOR);
    r = ov2640_set_byte_swap(&cameraOV2640, OV2640_BYTE_SWAP_UVUV);
    r = ov2640_set_dcw(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_sde(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_uv_adj(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_uv_avg(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_cmx(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_lp_dp(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_round(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_vertical_divider(&cameraOV2640, 0x00);
    r = ov2640_set_horizontal_divider(&cameraOV2640, 0x00);
    r = ov2640_set_zoom_speed(&cameraOV2640, 0x00);
    r = ov2640_set_quantization_scale_factor(&cameraOV2640, 0x0C);
    r = ov2640_set_sccb_master_speed(&cameraOV2640, 4);
    r = ov2640_set_address_auto_increase(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_sccb(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_sccb_master_clock_delay(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_sccb_master_access(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_sensor_pass_through_access(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_aec_enable(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_aec_sel(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_stat_sel(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_vfirst(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_yuv(&cameraOV2640, OV2640_BOOL_TRUE);
    r = ov2640_set_rgb(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_raw(&cameraOV2640, OV2640_BOOL_FALSE);
    r = ov2640_set_dp_selx(&cameraOV2640, 0x00);
    r = ov2640_set_dp_sely(&cameraOV2640, 0x00);
    r = ov2640_set_dsp_bypass(&cameraOV2640, OV2640_BOOL_FALSE);
#endif
}

void Camera::task() {
    vTaskDelayMS(1000);
    init();
    HAL_DCMI_Start_DMA(&cameraOV2640DCMI, DCMI_MODE_CONTINUOUS, (uint32_t)&cameraOV2640FrameBuffer, sizeof(cameraOV2640FrameBuffer) / sizeof(uint32_t));
    while (true) {
        while (!cameraOV2640FPS) {
            vTaskDelayMS(10);
        }
        // print("[camera] %dx%d %d fps\n", CAMERA_OV2640_IMAGE_WIDTH, CAMERA_OV2640_IMAGE_HEIGHT, cameraOV2640FPS);
#ifdef ROBOT_HAS_DISPLAY
        display.drawFrameBuffer(cameraOV2640FrameBuffer, CAMERA_OV2640_IMAGE_WIDTH, CAMERA_OV2640_IMAGE_HEIGHT);
#endif
        SCB_InvalidateDCache_by_Addr(&cameraOV2640FrameBuffer, sizeof(cameraOV2640FrameBuffer));
        cameraOV2640FPS = 0;
        vTaskDelayMS(50);
    }
}

void Camera::end() {}

void Camera::task(void* arg) {
    camera.task();
}

#ifdef ROBOT_HAS_CAMERA_OV2640

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hDCMI) {
    cameraOV2640FPS++;
}

void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef* hDCMI) {
    return;
}

void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef* hDCMI) {
    return;
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef* hDCMI) {
    return;
}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hDCMI) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_DCMI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*
    pinMode(CAMERA_OV2640_VSYNC_PIN, INPUT);
    pinMode(CAMERA_OV2640_HSYNC_PIN, INPUT);
    pinMode(CAMERA_OV2640_PCLK_PIN, INPUT);
    pinMode(CAMERA_OV2640_D0_PIN, INPUT);
    pinMode(CAMERA_OV2640_D1_PIN, INPUT);
    pinMode(CAMERA_OV2640_D2_PIN, INPUT);
    pinMode(CAMERA_OV2640_D3_PIN, INPUT);
    pinMode(CAMERA_OV2640_D4_PIN, INPUT);
    pinMode(CAMERA_OV2640_D5_PIN, INPUT);
    pinMode(CAMERA_OV2640_D6_PIN, INPUT);
    pinMode(CAMERA_OV2640_D7_PIN, INPUT);

    pin_function(digitalPinToPinName(CAMERA_OV2640_VSYNC_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_HSYNC_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_PCLK_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D0_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D1_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D2_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D3_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D4_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D5_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D6_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    pin_function(digitalPinToPinName(CAMERA_OV2640_D7_PIN), STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF13_DCMI));
    */

    cameraOV2640DMA.Instance = DMA1_Stream0;
    cameraOV2640DMA.Init.Request = DMA_REQUEST_DCMI;
    cameraOV2640DMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
    cameraOV2640DMA.Init.PeriphInc = DMA_PINC_DISABLE;
    cameraOV2640DMA.Init.MemInc = DMA_MINC_ENABLE;
    cameraOV2640DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    cameraOV2640DMA.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    cameraOV2640DMA.Init.Mode = DMA_CIRCULAR;
    cameraOV2640DMA.Init.Priority = DMA_PRIORITY_LOW;
    cameraOV2640DMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    HAL_DMA_Init(&cameraOV2640DMA);

    __HAL_LINKDMA(hDCMI, DMA_Handle, cameraOV2640DMA);

    HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);
}

void DMA1_Stream0_IRQHandler() {
    HAL_DMA_IRQHandler(&cameraOV2640DMA);
}

void DCMI_IRQHandler() {
    HAL_DCMI_IRQHandler(&cameraOV2640DCMI);
}

uint8_t ov2640_interface_sccb_init(void) {
    cameraOV2640Wire.setSCL(PB8);
    cameraOV2640Wire.setSDA(PB9);
    cameraOV2640Wire.begin();
    cameraOV2640Wire.setClock(100000);
    return 0;
}

uint8_t ov2640_interface_sccb_deinit() {
    cameraOV2640Wire.end();
    return 0;
}

uint8_t ov2640_interface_sccb_read(uint8_t addr, uint8_t reg, uint8_t* buf, uint16_t len) {
    addr >>= 1;
    uint8_t r;
    cameraOV2640Wire.beginTransmission(addr);
    cameraOV2640Wire.write(reg);
    if (r = cameraOV2640Wire.endTransmission(false)) {
        return r;
    }
    cameraOV2640Wire.requestFrom((int)addr, (int)len);
    for (int i = 0; i < len; i++) {
        if (cameraOV2640Wire.available()) {
            buf[i] = cameraOV2640Wire.read();
        } else {
            return 1;
        }
    }
    return 0;
}

uint8_t ov2640_interface_sccb_write(uint8_t addr, uint8_t reg, uint8_t* buf, uint16_t len) {
    addr >>= 1;
    uint8_t r;
    cameraOV2640Wire.beginTransmission(addr);
    cameraOV2640Wire.write(reg);
    cameraOV2640Wire.write(buf, len);
    if (r = cameraOV2640Wire.endTransmission()) {
        return r;
    }
    return 0;
}

uint8_t ov2640_interface_power_down_init() {
    uint32_t cameraOV2640TimerChannel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(CAMERA_OV2640_XCLK_PIN), PinMap_TIM));
    cameraOV2640Timer.setup(TIM1);
    cameraOV2640Timer.setMode(cameraOV2640TimerChannel, TIMER_OUTPUT_COMPARE_PWM1, digitalPinToPinName(CAMERA_OV2640_XCLK_PIN));
    cameraOV2640Timer.setPrescaleFactor(1);
    cameraOV2640Timer.setOverflow(20000000, HERTZ_FORMAT);
    cameraOV2640Timer.setCaptureCompare(cameraOV2640TimerChannel, 50, PERCENT_COMPARE_FORMAT);
    cameraOV2640Timer.resume();
    pinMode(CAMERA_OV2640_PWDN_PIN, OUTPUT);
    return 0;
}

uint8_t ov2640_interface_power_down_deinit() {
    cameraOV2640Timer.pause();
    return 0;
}

uint8_t ov2640_interface_power_down_write(uint8_t level) {
    digitalWrite(CAMERA_OV2640_PWDN_PIN, level);
    return 0;
}

uint8_t ov2640_interface_reset_init() {
    return 0;
}

uint8_t ov2640_interface_reset_deinit() {
    return 0;
}

uint8_t ov2640_interface_reset_write(uint8_t level) {
    return 0;
}

void ov2640_interface_delay_ms(uint32_t ms) {
    vTaskDelayMS(ms);
}

void ov2640_interface_debug_print(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vprint(format, args);
    va_end(args);
}

#endif

#endif
