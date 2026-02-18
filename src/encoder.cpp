
#include "encoder.h"

#include "motor.h"
#include "robot.h"

Encoder encoder = Encoder(ROBOT_MOTOR_ENCODER_I2C_ADDRESS, ROBOT_MOTOR_ENCODER_INTERRUPT_PIN);

Encoder::Encoder(int8_t _i2cAddress, int8_t _interruptPin) {
    i2cAddress = _i2cAddress;
    interruptPin = _interruptPin;
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    decoder = new rotaryDecoder(_i2cAddress);
#endif
}

Encoder::~Encoder() {}

void Encoder::begin() {
#if ROBOT_HAS_MOTOR_ENCODER_I2C
#if ROBOT_MOTOR_ENCODER_INTERRUPT_PIN
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(interruptPin, interrupt, FALLING);
#endif
    decoder->begin();
    if (decoder->isConnected()) {
        log_i("Encoder: 0x%02x connected", i2cAddress);
        xTaskCreatePinnedToCore(task, "encoder_task", 4096, NULL, 1, NULL, 0);
    } else {
        log_i("Encoder: not connected");
    }
#endif
}

void Encoder::task() {
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    decoder->reset();
    int count = 0;
    int debug = 0;
    while (true) {
        if (needUpdate) {
            log_i("Encoder: need update");
            needUpdate = false;
        }
        decoder->update();
        if (++count > 1024) {
            count = 0;
            bool updateSpeed = false;
            int value1 = decoder->getValue(0);
            if (-4 < value1 && value1 < 4) {
                value1 = 0;
            }
            if (motorLF.getEncoderSpeed() != value1) {
                motorLF.setEncoderSpeed(value1);
                updateSpeed = true;
            }
            int value2 = decoder->getValue(1);
            if (-4 < value2 && value2 < 4) {
                value2 = 0;
            }
            if (motorRF.getEncoderSpeed() != value2) {
                motorRF.setEncoderSpeed(value2);
                updateSpeed = true;
            }
            int value3 = decoder->getValue(2);
            if (-4 < value3 && value3 < 4) {
                value3 = 0;
            }
            if (motorLB.getEncoderSpeed() != value3) {
                motorLB.setEncoderSpeed(value3);
                updateSpeed = true;
            }
            int value4 = decoder->getValue(3);
            if (-4 < value4 && value4 < 4) {
                value4 = 0;
            }
            if (motorRB.getEncoderSpeed() != value4) {
                motorRB.setEncoderSpeed(value4);
                updateSpeed = true;
            }
            if (updateSpeed) {
                robot.needUpdateSpeed();
            }
            decoder->reset();
            if (++debug > 32) {
                debug = 0;
                log_i("Encoder: %d %d %d %d", value1, value2, value3, value4);
            }
        }
    }
#endif
}

void Encoder::task(void* arg) {
    encoder.task();
}

void Encoder::interrupt() {
    encoder.needUpdate = true;
}
