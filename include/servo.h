
#pragma once

#include <Arduino.h>

#include "print.h"

class Servo {
   public:
    int speed = 0;
    bool health = false;

    Servo(int n) {
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
        }
        channel1 = 1 + (n - 1) * 2;
        channel2 = 2 + (n - 1) * 2;
        ledcSetup(channel1, 20000, 8);
        ledcSetup(channel2, 20000, 8);
        ledcAttachPin(pin1, channel1);
        ledcAttachPin(pin2, channel2);
        health = true;
#endif
    }

    void setSpeed(int speed) {
        if (speed > 100) {
            speed = 100;
        } else if (speed < -100) {
            speed = -100;
        }
        if (this->speed != speed) {
            this->speed = speed;
#if ROBOT_HAS_SERVO_PWM
            if (speed == 0) {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, 0);
            } else {
                ledcWrite(channel1, speed > 0 ? maxSpeed : 0);
                ledcWrite(channel2, speed < 0 ? maxSpeed : 0);
            }
#endif
        }
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
