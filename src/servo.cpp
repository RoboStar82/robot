
#include "servo.h"

#if ROBOT_HAS_SERVO_PWM
ServoPWM servoLF = ServoPWM("LF", 18);
ServoPWM servoRF = ServoPWM("RF", 17);
ServoPWM servoLB = ServoPWM("LB", 39);
ServoPWM servoRB = ServoPWM("RB", 38);
#else
Servo servoLF = Servo("LF");
Servo servoRF = Servo("RF");
Servo servoLB = Servo("LB");
Servo servoRB = Servo("RB");
#endif

Servo::Servo(const char* _name) {
    name = _name;
}

Servo::~Servo() {}

void Servo::begin() {}

const char* Servo::getName() {
    return name;
}

void Servo::setMinAngle(int value) {
    minAngle = value;
}

void Servo::setMaxAngle(int value) {
    maxAngle = value;
}

void Servo::setAngle(int value, bool force) {
    int newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        log_i("Servo %s: %d", name, angle);
    }
}

ServoPWM::ServoPWM(const char* _name, uint8_t _pwmPin) : Servo(_name) {
    pwmPin = _pwmPin;
}

ServoPWM::~ServoPWM() {}

void ServoPWM::begin() {
    if (pwmPin) {
        ledcAttach(pwmPin, 50, 12);
        setAngle(angle, true);
    }
}

void ServoPWM::setAngle(int value, bool force) {
    int newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        ledcWrite(pwmPin, map(newAngle, minAngle, maxAngle, 500, 2500) * 4096 / 20000);
        log_i("Servo %s: %d", name, angle);
    }
}
