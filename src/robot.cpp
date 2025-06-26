
#include "robot.h"

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
    robot->updateSpeed();
}
