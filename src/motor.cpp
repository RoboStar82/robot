
#include "motor.h"

void Motor::setSpeed(int speed) {
    return;
}

MotorEncoder::MotorEncoder(uint8_t _encoderPin1, uint8_t _encoderPin2) {
    encoderPin1 = _encoderPin1;
    encoderPin2 = _encoderPin2;
}

void MotorEncoder::setSpeed(int speed) {
    return;
}

MotorPWM::MotorPWM(uint8_t _pwmPin1, uint8_t _pwmPin2) : MotorEncoder(0, 0) {
    pwmPin1 = _pwmPin1;
    pwmPin2 = _pwmPin2;
}

MotorPWM::MotorPWM(uint8_t _pwmPin1, uint8_t _pwmPin2, uint8_t _encoderPin1, uint8_t _encoderPin2) : MotorEncoder(_encoderPin1, _encoderPin2) {
    this->pwmPin1 = _pwmPin1;
    this->pwmPin2 = _pwmPin2;
}

void MotorPWM::setSpeed(int value) {
    return;
}
