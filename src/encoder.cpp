
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
    xTaskCreate(task, "encoder_task", 4096, NULL, 1, NULL);
#endif
}

void Encoder::task() {
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    decoder->reset();
    int count = 0;
    while (true) {
        decoder->update();
        if (++count > 1024) {
            count = 0;
            bool updateSpeed = false;
            int value;
            value = decoder->getValue(0);
            if (-4 < value && value < 4) {
                value = 0;
            } else {
                updateSpeed = true;
            }
            motorLF.setEncoderSpeed(value);
            value = decoder->getValue(1);
            if (-4 < value && value < 4) {
                value = 0;
            } else {
                updateSpeed = true;
            }
            motorRF.setEncoderSpeed(value);
            value = decoder->getValue(2);
            if (-4 < value && value < 4) {
                value = 0;
            } else {
                updateSpeed = true;
            }
            motorLB.setEncoderSpeed(value);
            value = decoder->getValue(3);
            if (-4 < value && value < 4) {
                value = 0;
            } else {
                updateSpeed = true;
            }
            motorRB.setEncoderSpeed(value);
            if (updateSpeed) {
                robot.needUpdateSpeed();
            }
            decoder->reset();
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
