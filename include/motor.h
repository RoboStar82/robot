
#pragma once

#include <Arduino.h>

#if ROBOT_HAS_MOTOR_I2C
#include <iarduino_I2C_Motor.h>
#endif

#include "print.h"

class Motor {
   public:
    int speed = 0;
    bool health = false;

    Motor(int n) {
#if ROBOT_HAS_MOTOR_I2C
        motor = new iarduino_I2C_Motor(0x40 + n);
        motor->setMagnet(motorMagnet);
        motor->setDirection(n % 2 == 0);
        motor->setStopNeutral(motorStopNeutral);
        motor->setNominalRPM(motorNominalRPM);
        motor->setReducer(motorReducer);
        motor->setVoltage(motorVoltage);
        health = motor->begin();
#endif
#if ROBOT_HAS_MOTOR_PWM
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
        if (speed > 127) {
            speed = 127;
        } else if (speed < -127) {
            speed = -127;
        }
        if (this->speed != speed) {
            this->speed = speed;
#if ROBOT_HAS_MOTOR_I2C
            if (speed == 0) {
                motor->setSpeed(0, MOT_RPM);
            } else {
                float absSpeed = ((float)(maxSpeed - minSpeed) * abs(speed) / 127 + minSpeed) * motorNominalRPM / 100;
                motor->setSpeed(speed > 0 ? absSpeed : -absSpeed, MOT_RPM);
            }
#endif
#if ROBOT_HAS_MOTOR_PWM
            if (speed == 0) {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, 0);
            } else {
                uint32_t absSpeed = ((float)(maxSpeed - minSpeed) * abs(speed) / 127 + minSpeed) * 255 / 100;
                ledcWrite(channel1, speed > 0 ? absSpeed : 0);
                ledcWrite(channel2, speed < 0 ? absSpeed : 0);
            }
#endif
        }
    }

    void setMinSpeed(int minSpeed) {
        if (minSpeed < 0) {
            minSpeed = 0;
        } else if (minSpeed > maxSpeed) {
            minSpeed = maxSpeed;
        }
        this->minSpeed = minSpeed;
    }

    void setMaxSpeed(int maxSpeed) {
        if (maxSpeed <= 0 || maxSpeed > 100) {
            maxSpeed = 100;
        } else if (maxSpeed < minSpeed) {
            maxSpeed = minSpeed;
        }
        this->maxSpeed = maxSpeed;
    }

   protected:
#if ROBOT_HAS_MOTOR_I2C
    iarduino_I2C_Motor *motor = nullptr;
#endif
#if ROBOT_HAS_MOTOR_PWM
    uint8_t channel1 = 0;
    uint8_t channel2 = 0;
#endif
    bool motorStopNeutral = true;
    uint16_t motorNominalRPM = 170;
    float motorVoltage = 12.0f;
    float motorReducer = 53.0f;
    uint8_t motorMagnet = 7;
    // 0..100
    int minSpeed = 40;
    int maxSpeed = 100;
};
