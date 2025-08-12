
#pragma once

#include <Arduino.h>

#include "print.h"

class Servo {
   public:
    int n = 0;
    int speed = 0;
    bool health = false;

    Servo(int n) {
        this->n = n;
#if ROBOT_HAS_SERVO_PWM
        int pin1 = 0;
        int pin2 = 0;
        switch (n) {
            case 1:
                pin1 = 27;
                pin2 = 13;
                break;
            case 2:
                pin1 = 4;
                pin2 = 2;
                break;
            case 3:
                pin1 = 17;
                pin2 = 12;
                break;
            case 4:
                pin1 = 15;
                pin2 = 14;
                break;
            case 5:
                pin1 = 26;
                break;
            case 6:
                pin1 = 25;
                break;
            case 7:
                pin1 = 33;
                break;
            case 8:
                pin1 = 32;
                break;
        }
        if (n > 4) {
            channel1 = n + 4;
            ledcSetup(channel1, 50, 16);
            ledcAttachPin(pin1, channel1);
        } else {
            if (pin1) {
                channel1 = 1 + (n - 1) * 2;
                ledcSetup(channel1, 20000, 8);
                ledcAttachPin(pin1, channel1);
            }
            if (pin2) {
                channel2 = 2 + (n - 1) * 2;
                ledcSetup(channel2, 20000, 8);
                ledcAttachPin(pin2, channel2);
            }
        }
        health = true;
#endif
    }

    void setSpeed(int speed) {
        if (speed > 1) {
            speed = 1;
        } else if (speed < -1) {
            speed = -1;
        }
        if (this->speed != speed) {
            this->speed = speed;
#if ROBOT_HAS_SERVO_PWM
            if (speed == 0) {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, 0);
            } else {
                ledcWrite(channel1, speed > 0 ? 2.55f * maxSpeed : 0);
                ledcWrite(channel2, speed < 0 ? 2.55f * maxSpeed : 0);
            }
#endif
        }
    }

    void setAngle(int angle) {
#if ROBOT_HAS_SERVO_PWM
        int ticks = map(angle, 0, 180, 500, 2500);
        int value = ticks * 65536 / 20000;
        ledcWrite(channel1, value);
#endif
    }

    void setMaxSpeed(int maxSpeed) {
        if (maxSpeed <= 0 || maxSpeed > 100) {
            maxSpeed = 100;
        }
        this->maxSpeed = maxSpeed;
    }

   protected:
#if ROBOT_HAS_SERVO_PWM
    uint8_t channel1 = 0;
    uint8_t channel2 = 0;
#endif
    // 0..100
    int maxSpeed = 100;
};
