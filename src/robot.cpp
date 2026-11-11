
#include <Arduino.h>

#include "config.h"

#include "robot.h"

#include "controller.h"
#include "motor.h"
#include "delay.h"
#include "print.h"

Robot robot;

Robot::Robot() {}

void Robot::begin() {
    if (!taskHandle) {
        xTaskCreate(task, "robot_task", 4096, NULL, 1, &taskHandle);
    }
}

void Robot::setSpeed(int speedLF, int speedRF, int speedLB, int speedRB) {
    motorLF.setSpeed(speedLF);
    motorRF.setSpeed(speedRF);
    motorLB.setSpeed(speedLB);
    motorRB.setSpeed(speedRB);
}

void Robot::autoStart() {
    if (!autoTaskHandle) {
        xTaskCreate(autoTask, "robot_auto_task", 4096, NULL, 1, &autoTaskHandle);
    }
}

void Robot::autoStop() {
    if (autoTaskHandle) {
        log_i("Robot: auto stop");
        vTaskDelete(autoTaskHandle);
        autoTaskHandle = nullptr;
        autoEnd();
    }
}

void Robot::autoEnd() {
    print("[Robot] auto end\n");
}

void Robot::updateSpeed() {
    ControllerState_t state = controller.getState();

    int ly = (int)4 * state.LY;
    int lx = (int)4 * state.LX;
    int ry = (int)-3 * state.RY;
    int rx = (int)3 * state.RX;

    if (abs(ry) > abs(rx) && ((ry > 0 && ly < 0) || (ry < 0 && ly > 0))) {
        // Разворот
        int ly = (int)3 * state.LY;
        int ry = (int)3 * state.RY;
        setSpeed(ly, ry, ly, ry);
    } else {
        // Обычное
        setSpeed(ly + lx + rx, ly - lx - rx, ly - lx + rx, ly + lx - rx);
    }
}

void Robot::updateServo() {
    updateCount();
}

void Robot::updateCount() {
}

void Robot::needUpdateSpeed() {
    RobotUpdate_t value = ROBOT_UPDATE_SPEED;
    xQueueSend(taskQueue, &value, 0);
}

void Robot::needUpdateServo() {
    RobotUpdate_t value = ROBOT_UPDATE_SERVO;
    xQueueSend(taskQueue, &value, 0);
}

void Robot::needUpdateAutoStart() {
    RobotUpdate_t value = ROBOT_UPDATE_START;
    xQueueSend(taskQueue, &value, 0);
}

void Robot::needUpdateAutoStop() {
    RobotUpdate_t value = ROBOT_UPDATE_STOP;
    xQueueSend(taskQueue, &value, 0);
}

void Robot::stop() {
    setSpeed(0, 0, 0, 0);
}

void Robot::task() {
    motorLF.begin();
    motorRF.begin();
    motorLB.begin();
    motorRB.begin();
    while (true) {
        RobotUpdate_t update;
        if (xQueueReceiveMS(taskQueue, &update, 50) == pdTRUE) {
            switch (update) {
                case ROBOT_UPDATE_STOP:
                    autoStop();
                    break;
                case ROBOT_UPDATE_START:
                    autoStart();
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

void Robot::autoTask() {
    print("[Robot] auto start\n");
}

void Robot::autoTask(void* arg) {
    robot.autoTask();
}
