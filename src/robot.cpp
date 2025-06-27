
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
    xTaskCreatePinnedToCore(robotBegin, "robot", 4096, NULL, 1, NULL, 1);
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
    int newSpeed = 0;
    int newSpeedLF = 0;
    int newSpeedRF = 0;
    int newSpeedLB = 0;
    int newSpeedRB = 0;
    int signX = 0;
    int signY = 0;
    int absX = 0;
    int absY = 0;

    // Левые моторы
    if (control->LX == 0 && control->LY == 0) {
        // D-Pad
        if (control->DX == 0 && control->DY == 0) {
            // Остановка
            newSpeedLF = 0;
            newSpeedLB = 0;
        } else {
            signX = control->DX;
            signY = control->DY;
            if (signY != 0) {
                // Движение вперед-назад
                // Поворот влево: левые колёса едут медленнее
                // Поворот вправо: левые колёса без корректировки
                newSpeed = signY * (signX >= 0 ? speedD : speedD2);
                newSpeedLF = newSpeed;
                newSpeedLB = newSpeed;
            } else {
                // Движение вправо-влево
                newSpeed = signX * speedD;
                newSpeedLF = -newSpeed;
                newSpeedLB = newSpeed;
            }
        }
    } else {
        signX = control->LX > 0 ? 1 : (control->LX < 0 ? -1 : 0);
        signY = control->LY > 0 ? 1 : (control->LY < 0 ? -1 : 0);
        absX = abs(control->LX);
        absY = abs(control->LY);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот влево: левые колёса едут медленнее
            // Поворот вправо: левые колёса без корректировки
            newSpeed = control->LY + (signX >= 0 ? 0 : signY) * control->LX;
            newSpeedLF = newSpeed;
            newSpeedLB = newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = control->LX - (signY > 0 ? signX : 0) * control->LY;
            newSpeedLF = -newSpeed;
            newSpeed = control->LX - (signY < 0 ? signX : 0) * control->LY;
            newSpeedLB = newSpeed;
        }
    }

    // Правые моторы
    if (control->RX == 0 && control->RY == 0) {
        // D-Pad
        if (control->DX == 0 && control->DY == 0) {
            // Остановка
            newSpeedRF = 0;
            newSpeedRB = 0;
        } else {
            signX = control->DX;
            signY = control->DY;
            if (signY != 0) {
                // Движение вперед-назад
                // Поворот вправо: правые колёса едут медленнее
                // Поворот влево: правые колёса без корректировки
                newSpeed = signY * (signX <= 0 ? speedD : speedD2);
                newSpeedRF = -newSpeed;
                newSpeedRB = -newSpeed;
            } else {
                // Движение вправо-влево
                newSpeed = signX * speedD;
                newSpeedRF = newSpeed;
                newSpeedRB = -newSpeed;
            }
        }
    } else {
        signX = control->RX > 0 ? 1 : (control->RX < 0 ? -1 : 0);
        signY = control->RY > 0 ? 1 : (control->RY < 0 ? -1 : 0);
        absX = abs(control->RX);
        absY = abs(control->RY);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот вправо: правые колёса едут медленнее
            // Поворот влево: правые колёса без корректировки
            newSpeed = control->RY + (signX <= 0 ? 0 : signY) * control->RX;
            newSpeedRF = -newSpeed;
            newSpeedRB = -newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = control->RX - (signY > 0 ? signX : 0) * control->RY;
            newSpeedRF = newSpeed;
            newSpeed = control->RX - (signY < 0 ? signX : 0) * control->RY;
            newSpeedRB = -newSpeed;
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
