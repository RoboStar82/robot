
#include "servo.h"

#if ROBOT_HAS_SERVO_1
ServoPWM servo1 = ServoPWM("1", ROBOT_SERVO_1_PIN);
#else
Servo servo1 = Servo("1");
#endif
#if ROBOT_HAS_SERVO_2
ServoPWM servo2 = ServoPWM("2", ROBOT_SERVO_2_PIN);
#else
Servo servo2 = Servo("2");
#endif
#if ROBOT_HAS_SERVO_3
ServoPWM servo3 = ServoPWM("3", ROBOT_SERVO_3_PIN);
#else
Servo servo3 = Servo("3");
#endif
#if ROBOT_HAS_SERVO_4
ServoPWM servo4 = ServoPWM("4", ROBOT_SERVO_4_PIN);
#else
Servo servo4 = Servo("4");
#endif
#if ROBOT_HAS_SERVO_5
ServoPWM servo5 = ServoPWM("5", ROBOT_SERVO_5_PIN);
#else
Servo servo5 = Servo("5");
#endif
#if ROBOT_HAS_SERVO_6
ServoPWM servo6 = ServoPWM("6", ROBOT_SERVO_6_PIN);
#else
Servo servo6 = Servo("6");
#endif
#if ROBOT_HAS_SERVO_7
ServoPWM servo7 = ServoPWM("7", ROBOT_SERVO_7_PIN);
#else
Servo servo7 = Servo("7");
#endif
#if ROBOT_HAS_SERVO_8
ServoPWM servo8 = ServoPWM("8", ROBOT_SERVO_8_PIN);
#else
Servo servo8 = Servo("8");
#endif

Servo::Servo(const char* _name) {
    name = _name;
}

Servo::~Servo() {}

void Servo::begin() {}

const char* Servo::getName() {
    return name;
}

void Servo::setMinAngle(float value) {
    minAngle = value;
}

void Servo::setMaxAngle(float value) {
    maxAngle = value;
}

void Servo::setAngle(float value, bool force) {
    float newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        log_i("Servo %s: %d", name, (int)angle);
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

void ServoPWM::setAngle(float value, bool force) {
    float newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        ledcWrite(pwmPin, map(newAngle, minAngle, maxAngle, 500, 2500) * 4096 / 20000);
        log_i("Servo %s: %d", name, (int)angle);
    }
}
