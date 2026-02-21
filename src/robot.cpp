
#include "robot.h"

#include "controller.h"
#include "lidar.h"
#include "motor.h"
#include "servo.h"
#include "encoder.h"

Robot robot;

Robot::Robot() {}

Robot::~Robot() {}

void Robot::begin() {
    if (!started) {
        xTaskCreate(task, "robot_task", 4096, NULL, 1, NULL);
        started = true;
    }
}

void Robot::setSpeed(int speedLF, int speedRF, int speedLB, int speedRB) {
    motorLF.setSpeed(speedLF);
    motorRF.setSpeed(speedRF);
    motorLB.setSpeed(speedLB);
    motorRB.setSpeed(speedRB);
}

void Robot::updateStart() {
#if ROBOT_HAS_LIDAR
    lidar.getDeviceInfo();
    lidar.getDeviceHealth();
    lidar.start();
#endif
}

void Robot::updateSpeed() {
    controller_state_t state = controller.getState();

    int ly = (int)36 * state.ly;
    int lx = (int)36 * state.lx;
    int ry = (int)30 * state.ry;
    int rx = (int)30 * state.rx;

    if (abs(ry) > abs(rx) && ((ry > 0 && ly < 0) || (ry < 0 && ly > 0))) {
        // Разворот
        int ly = (int)30 * state.ly;
        int ry = (int)30 * state.ry;
        setSpeed(ly, ry, ly, ry);
    } else {
        // Обычное
        setSpeed(ly + lx + rx, ly - lx - rx, ly - lx + rx, ly + lx - rx);
    }

    if (state.dy == 0) {
        if (state.ly != 0) {
            if (countLZ < 0) {
                motor1.setSpeed(ly + (rx >> 2));
            } else {
                motor1.setSpeed(0);
            }
            if (countRZ < 0) {
                motor2.setSpeed(ly - (rx >> 2));
            } else {
                motor2.setSpeed(0);
            }
        } else {
            motor1.setSpeed(0);
            motor2.setSpeed(0);
        }
        if (state.dx == 0) {
            motor3.setSpeed(0);
        } else if (state.dx > 0) {
            motor3.setSpeed(255);
        } else if (state.dx < 0) {
            motor3.setSpeed(-255);
        }
    } else if (state.dy > 0) {
        if (state.dx == 0) {
            motor1.setSpeed(255);
            motor2.setSpeed(255);
        } else if (state.dx > 0) {
            motor1.setSpeed(255);
            motor2.setSpeed(240);
        } else if (state.dx < 0) {
            motor1.setSpeed(240);
            motor2.setSpeed(255);
        }
    } else if (state.dy < 0) {
        if (state.dx == 0) {
            motor1.setSpeed(-255);
            motor2.setSpeed(-255);
        } else if (state.dx > 0) {
            motor1.setSpeed(-255);
            motor2.setSpeed(-240);
        } else if (state.dx < 0) {
            motor1.setSpeed(-240);
            motor2.setSpeed(-255);
        }
    }
}

void Robot::updateServo() {
    updateCount();
}

void Robot::needUpdateStart() {
    robot_update_t value = ROBOT_UPDATE_START;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateSpeed() {
    robot_update_t value = ROBOT_UPDATE_SPEED;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateServo() {
    robot_update_t value = ROBOT_UPDATE_SERVO;
    xQueueSend(needQueue, &value, 0);
}

void Robot::updateCount() {
    controller_state_t state = controller.getState();
    if (state.start || state.back) {
        return;
    }
    bool updateCountX = false;
    bool updateCountY = false;
    bool updateCountLZ = false;
    bool updateCountRZ = false;
    if (state.a) {
        countY = max(countY - 1, -64);
        updateCountY = true;
    }
    if (state.b) {
        countX = min(countX + 1, 64);
        updateCountX = true;
    }
    if (state.x) {
        countX = max(countX - 1, -64);
        updateCountX = true;
    }
    if (state.y) {
        countY = min(countY + 1, 64);
        updateCountY = true;
    }
    if (state.lz > 0) {
        countLZ = min(countLZ + 1, 4);
        updateCountLZ = true;
    } else if (state.lz < 0) {
        countLZ = max(countLZ - 1, -4);
        updateCountLZ = true;
    }
    if (state.rz > 0) {
        countRZ = min(countRZ + 1, 4);
        updateCountRZ = true;
    } else if (state.rz < 0) {
        countRZ = max(countRZ - 1, -4);
        updateCountRZ = true;
    }
    if (updateCountLZ) {
        servo1.setAngle(90.0f + 16.0f / 4.0f * countLZ);
    }
    if (updateCountRZ) {
        servo2.setAngle(90.0f - 16.0f / 4.0f * countRZ);
    }
    if (updateCountX) {
        servo3.setAngle(90.0f + 90.0f / 64.0f * countX);
        servo4.setAngle(90.0f + 90.0f / 64.0f * countX);
    }
    if (updateCountY) {
        servo5.setAngle(90.0f + 90.0f / 64.0f * countY);
        servo6.setAngle(90.0f + 90.0f / 64.0f * countY);
    }
}

void Robot::task() {
    motorLF.begin();
    motorRF.begin();
    motorLB.begin();
    motorRB.begin();
    motorCC.begin();
    motor1.begin();
    motor2.begin();
    motor3.begin();
    servoLF.begin();
    servoRF.begin();
    servoLB.begin();
    servoRB.begin();
    servo1.begin();
    servo2.begin();
    servo3.begin();
    servo4.begin();
    servo5.begin();
    servo6.begin();
    while (true) {
        robot_update_t update;
        if (xQueueReceive(needQueue, &update, 100)) {
            switch (update) {
                case ROBOT_UPDATE_START:
                    updateStart();
                    break;
                case ROBOT_UPDATE_SPEED:
                    updateSpeed();
                    break;
                case ROBOT_UPDATE_SERVO:
                    updateServo();
                    break;
            }
        } else {
            updateCount();
        }
    }
}

void Robot::task(void* arg) {
    robot.task();
}
