
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
