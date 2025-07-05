
#pragma once

#if ROBOT_HAS_MOTOR_I2C

#include <iarduino_I2C_Motor.h>

#endif

class Motor {
   public:
    int speed = 0;

    Motor(int n) {
#if ROBOT_HAS_MOTOR_I2C
        motor = new iarduino_I2C_Motor(0x40 + n);
        motor->setMagnet(motorMagnet);
        motor->setDirection(n % 2 == 0);
        motor->setStopNeutral(motorStopNeutral);
        motor->setNominalRPM(motorNominalRPM);
        motor->setReducer(motorReducer);
        motor->setVoltage(motorVoltage);
        motor->begin();
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
            case 4:
                pin1 = 15;
                pin2 = 14;
        }
        channel1 = 1 + (n - 1) * 2;
        channel2 = 2 + (n - 1) * 2;
        ledcSetup(channel1, 5000, 8);
        ledcSetup(channel2, 5000, 8);
        ledcAttachPin(pin1, channel1);
        ledcAttachPin(pin2, channel2);
#endif
    }

    void setSpeed(int speed) {
        if (this->speed != speed) {
            this->speed = speed;
#if ROBOT_HAS_MOTOR_I2C
            if (speed == 0) {
                motor->setSpeed(0, MOT_RPM);
            } else if (speed > 0) {
                motor->setSpeed((float)(motorMaxSpeed - motorMinSpeed) * speed / 7 + motorMinSpeed, MOT_RPM);
            } else if (speed < 0) {
                motor->setSpeed((float)(motorMaxSpeed - motorMinSpeed) * speed / 7 - motorMinSpeed, MOT_RPM);
            } else {
                motor->setSpeed(0, MOT_RPM);
            }
#endif
#if ROBOT_HAS_MOTOR_PWM
            speed = speed * 2;
            if (speed == 0) {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, 0);
            } else if (speed > 255) {
                ledcWrite(channel1, 255);
                ledcWrite(channel2, 0);
            } else if (speed < -255) {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, 255);
            } else if (speed > 0) {
                ledcWrite(channel1, speed);
                ledcWrite(channel2, 0);
            } else {
                ledcWrite(channel1, 0);
                ledcWrite(channel2, -speed);
            }
#endif
        }
    }

   protected:
#if ROBOT_HAS_MOTOR_I2C
    iarduino_I2C_Motor *motor = nullptr;
    bool motorStopNeutral = true;
    uint16_t motorNominalRPM = 170;
    float motorVoltage = 12.0f;
    float motorReducer = 53.0f;
    uint8_t motorMagnet = 7;
#endif
#if ROBOT_HAS_MOTOR_PWM
    uint8_t channel1 = 0;
    uint8_t channel2 = 0;
#endif
};
