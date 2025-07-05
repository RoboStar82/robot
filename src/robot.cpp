
#include "robot.h"

#define debugControl 1
#define debugMotor 1

Robot *robot = new Robot;

Robot *getRobot() {
    return robot;
}

void robotSetup() {
    robot->setController(getController());
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

void Robot::setController(Controller *controller) {
    this->controller = controller;
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
    if (controller->LX == 0 && controller->LY == 0) {
        // D-Pad
        if (controller->DX == 0 && controller->DY == 0) {
            // Остановка
            newSpeedLF = 0;
            newSpeedLB = 0;
        } else {
            signX = controller->DX;
            signY = controller->DY;
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
        signX = controller->LX > 0 ? 1 : (controller->LX < 0 ? -1 : 0);
        signY = controller->LY > 0 ? 1 : (controller->LY < 0 ? -1 : 0);
        absX = abs(controller->LX);
        absY = abs(controller->LY);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот влево: левые колёса едут медленнее
            // Поворот вправо: левые колёса без корректировки
            newSpeed = controller->LY + (signX >= 0 ? 0 : signY) * controller->LX;
            newSpeedLF = newSpeed;
            newSpeedLB = newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = controller->LX - (signY > 0 ? signX : 0) * controller->LY;
            newSpeedLF = -newSpeed;
            newSpeed = controller->LX + (signY < 0 ? signX : 0) * controller->LY;
            newSpeedLB = newSpeed;
        }
    }

    // Правые моторы
    if (controller->RX == 0 && controller->RY == 0) {
        // D-Pad
        if (controller->DX == 0 && controller->DY == 0) {
            // Остановка
            newSpeedRF = 0;
            newSpeedRB = 0;
        } else {
            signX = controller->DX;
            signY = controller->DY;
            if (signY != 0) {
                // Движение вперед-назад
                // Поворот вправо: правые колёса едут медленнее
                // Поворот влево: правые колёса без корректировки
                newSpeed = signY * (signX <= 0 ? speedD : speedD2);
                newSpeedRF = newSpeed;
                newSpeedRB = newSpeed;
            } else {
                // Движение вправо-влево
                newSpeed = signX * speedD;
                newSpeedRF = newSpeed;
                newSpeedRB = -newSpeed;
            }
        }
    } else {
        signX = controller->RX > 0 ? 1 : (controller->RX < 0 ? -1 : 0);
        signY = controller->RY > 0 ? 1 : (controller->RY < 0 ? -1 : 0);
        absX = abs(controller->RX);
        absY = abs(controller->RY);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот вправо: правые колёса едут медленнее
            // Поворот влево: правые колёса без корректировки
            newSpeed = controller->RY - (signX <= 0 ? 0 : signY) * controller->RX;
            newSpeedRF = newSpeed;
            newSpeedRB = newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = controller->RX - (signY > 0 ? signX : 0) * controller->RY;
            newSpeedRF = newSpeed;
            newSpeed = controller->RX + (signY < 0 ? signX : 0) * controller->RY;
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

void Controller::onChangeStart() {
#if debugControl
    println("V: robot: start");
#endif
    lidarStart();
}

void Controller::onChangeBack() {
#if debugControl
    println("V: robot: back");
#endif
    if (robot->autoMode) {
        robot->autoMode = 0;
    } else {
        lidarStop();
    }
}

void Controller::onChangeA() {
#if debugControl
    println("V: robot: A");
#endif
    robot->autoMode = 'A';
}

void Controller::onChangeB() {
#if debugControl
    println("V: robot: B");
#endif
    robot->autoMode = 'B';
}

void Controller::onChangeX() {
#if debugControl
    println("V: robot: X");
#endif
    robot->autoMode = 'X';
}

void Controller::onChangeY() {
#if debugControl
    println("V: robot: Y");
#endif
    robot->autoMode = 'Y';
}

void Controller::onChangeZ() {
#if debugControl
    debug("V: robot: Z: %d %d\n", robot->controller->LZ, robot->controller->RZ);
#endif
}

void Controller::onChangeXY() {
    robot->autoMode = 0;
    robot->updateSpeed();
}
