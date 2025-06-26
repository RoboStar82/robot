
#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <iarduino_I2C_Motor.h>

#include "ble.h"
#include "bmx.h"
#include "lidar.h"
#include "print.h"

class Robot {
   public:
    RobotControl *control = nullptr;

    char autoMode = 0;

    bool motorStopNeutral = true;
    uint16_t motorNominalRPM = 170;
    float motorVoltage = 12.0f;
    float motorReducer = 53.0f;
    uint8_t motorMagnet = 7;

    iarduino_I2C_Motor *motorLF = nullptr;
    iarduino_I2C_Motor *motorRF = nullptr;
    iarduino_I2C_Motor *motorLB = nullptr;
    iarduino_I2C_Motor *motorRB = nullptr;
    iarduino_I2C_Motor *motors[4] = {nullptr, nullptr, nullptr, nullptr};

    // Состояние моторов

    bool healthLF = false;
    bool healthRF = false;
    bool healthLB = false;
    bool healthRB = false;

    // Скорости моторов

    // Left front
    int speedLF = 0;
    // Right front
    int speedRF = 0;
    // Left back
    int speedLB = 0;
    // Right back
    int speedRB = 0;

    int motorMinSpeed = 20;
    int motorMaxSpeed = 80;

    int speedD = 5;

    void setMotors(iarduino_I2C_Motor *motorLF, iarduino_I2C_Motor *motorRF, iarduino_I2C_Motor *motorLB, iarduino_I2C_Motor *motorRB) {
        this->motorLF = motorLF;
        this->motorRF = motorRF;
        this->motorLB = motorLB;
        this->motorRB = motorRB;
        motors[0] = motorLF;
        motors[1] = motorRF;
        motors[2] = motorLB;
        motors[3] = motorRB;
        // Подготовка моторов
        healthLF = motorLF->begin();
        healthRF = motorRF->begin();
        healthLB = motorLB->begin();
        healthRB = motorRB->begin();
        for (int i = 0; i < 4; i++) {
            iarduino_I2C_Motor *motor = motors[i];
            motor->setMagnet(motorMagnet);
            motor->setDirection(i % 2 == 0);
            motor->setStopNeutral(motorStopNeutral);
            motor->setNominalRPM(motorNominalRPM);
            motor->setReducer(motorReducer);
            motor->setVoltage(motorVoltage);
        }
    }

    void setControl(RobotControl *control) {
        this->control = control;
    }

    void setMotorSpeed(iarduino_I2C_Motor *motor, int speed) {
        if (speed == 0) {
            motor->setSpeed(0, MOT_RPM);
        } else if (speed > 0) {
            motor->setSpeed((float)(motorMaxSpeed - motorMinSpeed) * speed / 7 + motorMinSpeed, MOT_RPM);
        } else if (speed < 0) {
            motor->setSpeed((float)(motorMaxSpeed - motorMinSpeed) * speed / 7 - motorMinSpeed, MOT_RPM);
        } else {
            motor->setSpeed(0, MOT_RPM);
        }
    }

    void updateSpeed() {
        int newSpeedLF = 0;
        int newSpeedRF = 0;
        int newSpeedLB = 0;
        int newSpeedRB = 0;
        int absX;
        int absY;
        absX = abs(control->LX);
        absY = abs(control->LY);
        if (absX == 0 && absY == 0) {
            if (control->DX == 0 && control->DY == 0) {
                // Остановка
                newSpeedLF = 0;
                newSpeedLB = 0;
            } else if (control->DY != 0) {
                if (control->DX == 0) {
                    // Движение вперед-назад
                    newSpeedLF = control->DY < 0 ? -speedD : speedD;
                    newSpeedLB = control->DY < 0 ? -speedD : speedD;
                } else {
                    // Движение вперед-назад
                    newSpeedLF = control->DY < 0 ? -speedD : speedD;
                    newSpeedLB = control->DY < 0 ? -speedD : speedD;
                }
            } else if (control->DX != 0) {
                // Движение вправо-влево
                newSpeedLF = control->DX < 0 ? speedD : -speedD;
                newSpeedLB = control->DX < 0 ? -speedD : speedD;
            } else {
                // Остановка
                newSpeedLF = 0;
                newSpeedLB = 0;
            }
        } else if (absX <= absY) {
            // Движение вперед-назад
            newSpeedLF = control->LY;
            newSpeedLB = control->LY;
        } else {
            // Движение вправо-влево
            newSpeedLF = -control->LX;
            newSpeedLB = control->LX;
        }
        absX = abs(control->RX);
        absY = abs(control->RY);
        if (absX == 0 && absY == 0) {
            if (control->DX == 0 && control->DY == 0) {
                // Остановка
                newSpeedRF = 0;
                newSpeedRB = 0;
            } else if (control->DY != 0) {
                if (control->DX == 0) {
                    // Движение вперед-назад
                    newSpeedRF = control->DY < 0 ? -speedD : speedD;
                    newSpeedRB = control->DY < 0 ? -speedD : speedD;
                } else {
                    // Движение вперед-назад
                    newSpeedRF = control->DY < 0 ? -speedD : speedD;
                    newSpeedRB = control->DY < 0 ? -speedD : speedD;
                }
            } else if (control->DX != 0) {
                // Движение вправо-влево
                newSpeedRF = control->DX < 0 ? -speedD : speedD;
                newSpeedRB = control->DX < 0 ? speedD : -speedD;
            } else {
                // Остановка
                newSpeedRF = 0;
                newSpeedRB = 0;
            }
        } else if (absX <= absY) {
            // Движение вперед-назад
            newSpeedRF = control->RY;
            newSpeedRB = control->RY;
        } else {
            // Движение вправо-влево
            newSpeedRF = control->RX;
            newSpeedRB = -control->RX;
        }
        if (speedLF != newSpeedLF) {
            speedLF = newSpeedLF;
#if debugMotor
            debug("V: motor: LF: %d\n", speedLF);
#endif
            setMotorSpeed(motorLF, speedLF);
        }
        if (speedRF != newSpeedRF) {
            speedRF = newSpeedRF;
#if debugMotor
            debug("V: motor: RF: %d\n", speedRF);
#endif
            setMotorSpeed(motorRF, speedRF);
        }
        if (speedLB != newSpeedLB) {
            speedLB = newSpeedLB;
#if debugMotor
            debug("V: motor: LB: %d\n", speedLB);
#endif
            setMotorSpeed(motorLB, speedLB);
        }
        if (speedRB != newSpeedRB) {
            speedRB = newSpeedRB;
#if debugMotor
            debug("V: motor: RB: %d\n", speedRB);
#endif
            setMotorSpeed(motorRB, speedRB);
        }
    }

    void loop() {
        delay(1000);
    }
};

void robotSetup();

void robotBegin(void *params);

void robotLoop();
