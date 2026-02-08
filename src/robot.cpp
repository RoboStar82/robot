
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
        if (raise < 0) {
            if (state.ly == 0) {
                motorCC.setSpeed(0);
            } else if (state.ly > 4) {
                motorCC.setSpeed(255);
            } else if (state.ly > 0) {
                motorCC.setSpeed(state.ly * 36);
            } else if (state.ly < -4) {
                motorCC.setSpeed(-255);
            } else if (state.ly < 0) {
                motorCC.setSpeed(state.ly * 36);
            }
        } else {
            motorCC.setSpeed(0);
        }
    } else if (state.dy > 0) {
        motorCC.setSpeed(255);
    } else if (state.dy < 0) {
        motorCC.setSpeed(-255);
    }
}

void Robot::updateServo() {
    controller_state_t state = controller.getState();
    bool updateRaise = false;
    if (state.lz > 0) {
        raise--;
        updateRaise = true;
    } else if (state.lz < 0) {
        raise++;
        updateRaise = true;
    }
    if (updateRaise) {
        servoLF.setAngle(90.0f + 2.0f * raise);
        servoRF.setAngle(90.0f - 2.0f * raise);
        servoLB.setAngle(90.0f + 2.0f * raise);
        servoRB.setAngle(90.0f - 2.0f * raise);
    }
}

void Robot::needUpdateSpeed() {
    robot_update_t value = ROBOT_UPDATE_SPEED;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateServo() {
    robot_update_t value = ROBOT_UPDATE_SERVO;
    xQueueSend(needQueue, &value, 0);
}

void Robot::task() {
    vTaskDelay(1000);
    motorLF.begin();
    motorRF.begin();
    motorLB.begin();
    motorRB.begin();
    motorCC.begin();
    servoLF.begin();
    servoRF.begin();
    servoLB.begin();
    servoRB.begin();
    vTaskDelay(1000);
    while (true) {
        robot_update_t update;
        if (xQueueReceive(needQueue, &update, 1000)) {
            switch (update) {
                case ROBOT_UPDATE_SPEED:
                    updateSpeed();
                    break;
                case ROBOT_UPDATE_SERVO:
                    updateServo();
                    break;
            }
        }
    }
}

void Robot::task(void* arg) {
    robot.task();
}
