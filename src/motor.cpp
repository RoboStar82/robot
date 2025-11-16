
#include "motor.h"

Motor::Motor(const char* _name) {
    name = _name;
}

void Motor::begin() {
    return;
}

void Motor::setSpeed(int value) {
    return;
}

void Motor::setMinSpeed(uint value) {
    return;
}

void Motor::setMaxSpeed(uint value) {
    return;
}

MotorEncoder::MotorEncoder(const char* _name, uint8_t _encoderPin1, uint8_t _encoderPin2) : Motor(_name) {
    name = _name;
    encoderPin1 = _encoderPin1;
    encoderPin2 = _encoderPin2;
}

void MotorEncoder::begin() {
    return;
}

void MotorEncoder::setSpeed(int value) {
    return;
}

void MotorEncoder::setMinSpeed(uint value) {
    return;
}

void MotorEncoder::setMaxSpeed(uint value) {
    return;
}

MotorPWM::MotorPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2) : MotorEncoder(_name, 0, 0) {
    name = _name;
    pwmPin1 = _pwmPin1;
    pwmPin2 = _pwmPin2;
}

MotorPWM::MotorPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2, uint8_t _encoderPin1, uint8_t _encoderPin2) : MotorEncoder(_name, _encoderPin1, _encoderPin2) {
    name = _name;
    pwmPin1 = _pwmPin1;
    pwmPin2 = _pwmPin2;
}

void MotorPWM::begin() {
    if (pwmPin1) {
        ledcAttach(pwmPin1, 80000, 8);
    }
    if (pwmPin2) {
        ledcAttach(pwmPin2, 80000, 8);
    }
}

void MotorPWM::setSpeed(int value) {
    if (pwmPin1 && pwmPin2) {
        bool back = value < 0;
        uint speed = back ? -value : value;
        if (speed) {
            if (minSpeed < maxSpeed) {
                if (speed < 0xff) {
                    speed = minSpeed + speed * (maxSpeed - minSpeed) / 0xff;
                } else {
                    speed = maxSpeed;
                }
            }
        }
        log_i("Motor %s: %d", name, back ? -speed : speed);
        ledcWrite(pwmPin1, back ? 0 : speed);
        ledcWrite(pwmPin2, back ? speed : 0);
    }
}

void MotorPWM::setMinSpeed(uint value) {
    minSpeed = value;
}

void MotorPWM::setMaxSpeed(uint value) {
    maxSpeed = value;
}
