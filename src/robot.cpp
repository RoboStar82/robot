
#include "robot.h"

#include "controller.h"
#include "motor.h"
#include "servo.h"

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
        motor1.setSpeed(0);
    } else if (state.dy > 0) {
        motor1.setSpeed(255);
    } else if (state.dy < 0) {
        motor1.setSpeed(-255);
    }
    if (state.dx == 0) {
        motor2.setSpeed(0);
    } else if (state.dx > 0) {
        motor2.setSpeed(255);
    } else if (state.dx < 0) {
        motor2.setSpeed(-255);
    }
}

void Robot::updateServo() {
    updateCount();
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
        countY = max(countY - 8, -256);
        updateCountY = true;
    }
    if (state.b) {
        countX = min(countX + 8, 256);
        updateCountX = true;
    }
    if (state.x) {
        countX = max(countX - 8, -256);
        updateCountX = true;
    }
    if (state.y) {
        countY = min(countY + 8, 256);
        updateCountY = true;
    }
    if (state.lz > 0) {
        countLZ = min(countLZ + 8, 256);
        updateCountLZ = true;
    } else if (state.lz < 0) {
        countLZ = max(countLZ - 8, 256);
        updateCountLZ = true;
    }
    if (state.rz > 0) {
        countRZ = min(countRZ + 8, 256);
        updateCountRZ = true;
    } else if (state.rz < 0) {
        countRZ = max(countRZ - 8, 256);
        updateCountRZ = true;
    }
    if (updateCountX) {
        servo1.setAngle(90.0f + 90.0f / 256.0f * countX);
    }
    if (updateCountY) {
        servo2.setAngle(90.0f + 90.0f / 256.0f * countY);
    }
    if (updateCountLZ) {
        servo3.setAngle(90.0f + 90.0f / 256.0f * countLZ);
    }
    if (updateCountRZ) {
        servo4.setAngle(90.0f + 90.0f / 256.0f * countRZ);
    }
}

void Robot::task() {
    vTaskDelay(1000);
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
    vTaskDelay(1000);
    while (true) {
        robot_update_t update;
        if (xQueueReceive(needQueue, &update, 200)) {
            switch (update) {
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
