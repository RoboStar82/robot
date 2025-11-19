
#include "motor.h"

Motor::Motor(const char* _name) {
    name = _name;
}

void Motor::begin() {}

const char* Motor::getName() {
    return name;
}

void Motor::setMinSpeed(uint value) {
    minSpeed = value;
}

void Motor::setMaxSpeed(uint value) {
    maxSpeed = value;
}

void Motor::setSpeed(int value) {
    bool back = value < 0;
    uint absSpeed = back ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < minSpeed) {
                absSpeed = minSpeed;
            } else if (absSpeed > maxSpeed) {
                absSpeed = maxSpeed;
            }
        }
    }
    speed = back ? -speed : speed;
    log_i("Motor %s: %d", name, speed);
}

MotorEncoder::MotorEncoder(const char* _name, uint8_t _encoderPin1, uint8_t _encoderPin2) : Motor(_name) {
    encoderPin1 = _encoderPin1;
    encoderPin2 = _encoderPin2;
}

void MotorEncoder::begin() {}

MotorPWM::MotorPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2) : Motor(_name) {
    pwmPin1 = _pwmPin1;
    pwmPin2 = _pwmPin2;
}

void MotorPWM::begin() {
    if (pwmPin1) {
        ledcAttach(pwmPin1, 20000, 8);
    }
    if (pwmPin2) {
        ledcAttach(pwmPin2, 20000, 8);
    }
}

void MotorPWM::setSpeed(int value) {
    bool back = value < 0;
    uint absSpeed = back ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < 0xff) {
                absSpeed = minSpeed + absSpeed * (maxSpeed - minSpeed) / 0xff;
            } else {
                absSpeed = maxSpeed;
            }
        }
    }
    speed = back ? -speed : speed;
    log_i("Motor %s: %d", name, speed);
    if (pwmPin1 && pwmPin2) {
        ledcWrite(pwmPin1, back ? 0 : absSpeed);
        ledcWrite(pwmPin2, back ? absSpeed : 0);
    }
}

MotorMCPWM::MotorMCPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2) : MotorPWM(_name, _pwmPin1, _pwmPin2) {}

void MotorMCPWM::begin() {
}

void MotorMCPWM::setSpeed(int value) {
    bool back = value < 0;
    uint absSpeed = back ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < 0xff) {
                absSpeed = minSpeed + absSpeed * (maxSpeed - minSpeed) / 0xff;
            } else {
                absSpeed = maxSpeed;
            }
        }
    }
    speed = back ? -speed : speed;
    log_i("Motor %s: %d", name, speed);
    if (pwmPin1 && pwmPin2) {
    }
}
