
#include "robot.h"

#define debugControl 1
#define debugMotor 1

Robot *robot = new Robot;

Robot *getRobot() {
    return robot;
}

void robotSetup() {
    robot->setControl(getRobotControl());
    robot->setMotors(new iarduino_I2C_Motor(0x41), new iarduino_I2C_Motor(0x42), new iarduino_I2C_Motor(0x43), new iarduino_I2C_Motor(0x44));
    uint8_t *robotHealth = getRobotHealth();
    robotHealth[0] |= (robot->healthLF ? 1 : 0) << 0;
    robotHealth[0] |= (robot->healthRF ? 1 : 0) << 1;
    robotHealth[0] |= (robot->healthLB ? 1 : 0) << 2;
    robotHealth[0] |= (robot->healthRB ? 1 : 0) << 3;
    BLECharacteristic *robotHealthCharacteristic = getRobotHealthCharacteristic();
    robotHealthCharacteristic->setValue(robotHealth, 1);
    robotHealthCharacteristic->notify();
    xTaskCreatePinnedToCore(robotBegin, "robot", 8192, NULL, 1, NULL, 1);
}

void robotBegin(void *params) {
    while (true) {
        robot->loop();
    }
}

void robotLoop() {
    robot->loop();
}

// Робот

void Robot::setControl(RobotControl *control) {
    this->control = control;
};

void Robot::setMotors(iarduino_I2C_Motor *motorLF, iarduino_I2C_Motor *motorRF, iarduino_I2C_Motor *motorLB, iarduino_I2C_Motor *motorRB) {
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

void Robot::setMotorSpeed(iarduino_I2C_Motor *motor, int speed) {
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

void Robot::updateSpeed() {
    int newSpeedLF = 0;
    int newSpeedRF = 0;
    int newSpeedLB = 0;
    int newSpeedRB = 0;
    int absX;
    int absY;

    // Левые моторы
    absX = abs(control->LX);
    absY = abs(control->LY);
    if (absX == 0 && absY == 0) {
        // D-Pad
        if (control->DX == 0 && control->DY == 0) {
            // Остановка
            newSpeedLF = 0;
            newSpeedLB = 0;
        } else if (control->DX != 0 && control->DY != 0) {
            // Движение наискосок
            if (control->DX > 0) {
                // При повороте вправо левые колёса едут быстрее
                newSpeedLF = control->DY < 0 ? -speedD : speedD;
                newSpeedLB = control->DY < 0 ? -speedD : speedD;
            } else {
                // При повороте влево левые колёса едут медленнее
                newSpeedLF = control->DY < 0 ? -speedD2 : speedD2;
                newSpeedLB = control->DY < 0 ? -speedD2 : speedD2;
            }
        } else if (control->DY != 0) {
            // Движение вперед-назад
            newSpeedLF = control->DY < 0 ? -speedD : speedD;
            newSpeedLB = control->DY < 0 ? -speedD : speedD;
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
        if (control->LX < 0) {
            // При повороте влево левые колёса едут медленнее
            if (control->LY > 0) {
                newSpeedLF = control->LY + control->LX;
                newSpeedLB = control->LY + control->LX;
            } else {
                newSpeedLF = control->LY - control->LX;
                newSpeedLB = control->LY - control->LX;
            }
        } else {
            newSpeedLF = control->LY;
            newSpeedLB = control->LY;
        }
    } else {
        // Движение вправо-влево
        if (control->LY > 0) {
            // При повороте вверх верхнее колесо едет медленнее
            if (control->LX > 0) {
                newSpeedLF = -control->LX - control->LY;
            } else {
                newSpeedLF = -control->LX + control->LY;
            }
        } else {
            newSpeedLF = -control->LX;
        }
        if (control->LY < 0) {
            // При повороте вниз нижнее колесо едет медленнее
            if (control->LX > 0) {
                newSpeedLB = control->LX + control->LY;
            } else {
                newSpeedLB = control->LX - control->LY;
            }
        } else {
            newSpeedLB = control->LX;
        }
    }

    // Правые моторы
    absX = abs(control->RX);
    absY = abs(control->RY);
    if (absX == 0 && absY == 0) {
        // D-Pad
        if (control->DX == 0 && control->DY == 0) {
            // Остановка
            newSpeedRF = 0;
            newSpeedRB = 0;
        } else if (control->DX != 0 && control->DY != 0) {
            // Движение наискосок
            if (control->DX < 0) {
                // При повороте влево правые колёса едут быстрее
                newSpeedRF = control->DY < 0 ? -speedD : speedD;
                newSpeedRB = control->DY < 0 ? -speedD : speedD;
            } else {
                // При повороте вправо правые колёса едут медленнее
                newSpeedRF = control->DY < 0 ? -speedD2 : speedD2;
                newSpeedRB = control->DY < 0 ? -speedD2 : speedD2;
            }
        } else if (control->DY != 0) {
            // Движение вперед-назад
            newSpeedRF = control->DY < 0 ? -speedD : speedD;
            newSpeedRB = control->DY < 0 ? -speedD : speedD;
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
        if (control->RX > 0) {
            // При повороте вправо правые колёса едут медленнее
            if (control->RY > 0) {
                newSpeedRF = control->RY - control->RX;
                newSpeedRB = control->RY - control->RX;
            } else {
                newSpeedRF = control->RY + control->RX;
                newSpeedRB = control->RY + control->RX;
            }
        } else {
            newSpeedRF = control->RY;
            newSpeedRB = control->RY;
        }
    } else {
        // Движение вправо-влево
        if (control->RY > 0) {
            // При повороте вверх верхнее колесо едет медленнее
            if (control->RX > 0) {
                newSpeedRF = control->RX - control->RY;
            } else {
                newSpeedRF = control->RX + control->RY;
            }
        } else {
            newSpeedRF = control->RX;
        }
        if (control->RY < 0) {
            // При повороте вниз нижнее колесо едет медленнее
            if (control->RX > 0) {
                newSpeedRB = -control->RX - control->RY;
            } else {
                newSpeedRB = -control->RX + control->RY;
            }
        } else {
            newSpeedRB = -control->RX;
        }
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

void Robot::loop() {
    delay(1000);
}

// Обработка изменения состояний кнопок

void RobotControl::onChangeStart() {
#if debugControl
    println("V: robot: start");
#endif
    lidarStart();
}

void RobotControl::onChangeBack() {
#if debugControl
    println("V: robot: back");
#endif
    lidarStop();
}

void RobotControl::onChangeA() {
#if debugControl
    println("V: robot: A");
#endif
    robot->autoMode = 'A';
}

void RobotControl::onChangeB() {
#if debugControl
    println("V: robot: B");
#endif
    robot->autoMode = 'B';
}

void RobotControl::onChangeX() {
#if debugControl
    println("V: robot: X");
#endif
    robot->autoMode = 'X';
}

void RobotControl::onChangeY() {
#if debugControl
    println("V: robot: Y");
#endif
    robot->autoMode = 'Y';
}

void RobotControl::onChangeZ() {
#if debugControl
    debug("V: robot: Z: %d %d\n", robot->control->LZ, robot->control->RZ);
#endif
}

void RobotControl::onChangeXY() {
    robot->autoMode = 0;
    robot->updateSpeed();
}
