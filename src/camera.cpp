
#include <Arduino.h>
#include <FreeRTOS.h>
#include <HardwareTimer.h>
#include <Wire.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_CAMERA

#include "camera.h"
#include "delay.h"
#include "print.h"

Camera camera;

#ifdef ROBOT_HAS_CAMERA_OV2640

#include "driver_ov2640_interface.h"

TwoWire cameraOV2640Wire;
HardwareTimer cameraOV2640Timer;
DMA_HandleTypeDef cameraOV2640DMA;
DCMI_HandleTypeDef cameraOV2640DCMI;

// __attribute__((section(".rama_d1")))
uint16_t cameraOV2640FrameBuffer[160][120] = {0};

bool cameraOV2640HasFrame = false;

void cameraOV2640FrameEvent(DCMI_HandleTypeDef* hDCMI) {
    cameraOV2640HasFrame = true;
}

void cameraOV2640DMAInit() {
    memset(cameraOV2640FrameBuffer, sizeof(uint16_t), 160 * 120);

    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

void cameraOV2640MSPInit(DCMI_HandleTypeDef* hDCMI) {
    __HAL_RCC_DCMI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

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

    uint8_t r = HAL_DMA_Init(&cameraOV2640DMA);

    __HAL_LINKDMA(&cameraOV2640DCMI, DMA_Handle, cameraOV2640DMA);

    HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);

    print("[camera] MSP init: %d\n", r);
}

void cameraOV2640DCMIInit() {
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

    cameraOV2640DCMI.MspInitCallback = cameraOV2640MSPInit;
    cameraOV2640DCMI.FrameEventCallback = cameraOV2640FrameEvent;

    uint8_t r = HAL_DCMI_Init(&cameraOV2640DCMI);

    print("[camera] DCMI init: %d\n", r);
}

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
#endif

Camera::Camera() {}

Camera::~Camera() {}

void Camera::begin() {
    if (!taskStarted) {
        xTaskCreate(task, "camera_task", 4096, NULL, 1, &taskStarted);
    }
}

void Camera::task() {
    vTaskDelayMS(1000);

    uint8_t r = 0;
    if (r = ov2640_init(&cameraOV2640)) {
        print("[camera] ov2640_init: %d\n", r);
    }
    if (r = ov2640_table_init(&cameraOV2640)) {
        print("[camera] ov2640_table_init: %d\n", r);
    }
    if (r = ov2640_table_rgb565_init(&cameraOV2640)) {
        print("[camera] ov2640_table_rgb565_init: %d\n", r);
    }
    if (r = ov2640_set_output_width(&cameraOV2640, 160 / 4)) {
        print("[camera] ov2640_set_output_width: %d\n", r);
    }
    if (r = ov2640_set_output_height(&cameraOV2640, 120 / 4)) {
        print("[camera] ov2640_set_output_height: %d\n", r);
    }

    cameraOV2640DMAInit();
    cameraOV2640DCMIInit();

    r = HAL_DCMI_Start_DMA(&cameraOV2640DCMI, DCMI_MODE_CONTINUOUS, (uint32_t)&cameraOV2640FrameBuffer, 160 * 120 / 2);

    print("[camera] DCMI DMA init: %d\n", r);

    while (true) {
        print("[camera] %d %02x%02x%02x%02x%02x%02x%02x%02x\n", cameraOV2640HasFrame, cameraOV2640FrameBuffer[0][0], cameraOV2640FrameBuffer[1][1], cameraOV2640FrameBuffer[2][2], cameraOV2640FrameBuffer[3][3], cameraOV2640FrameBuffer[4][4], cameraOV2640FrameBuffer[5][5], cameraOV2640FrameBuffer[6][6], cameraOV2640FrameBuffer[7][7]);
        // SCB_InvalidateDCache_by_Addr(&cameraOV2640FrameBuffer, 256 * 256 / 2);
        vTaskDelayMS(1000);
    }
}

void Camera::end() {}

void Camera::task(void* arg) {
    camera.task();
}

#ifdef ROBOT_HAS_CAMERA_OV2640

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
    RobotSerial.vprintf(format, args);
    va_end(args);
}

#endif

#endif
