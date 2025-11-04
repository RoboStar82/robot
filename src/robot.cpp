
#include "robot.h"

#include "controller.h"

Robot robot;

void Robot::begin() {}

void Robot::setSpeed(int8_t speedLF, int8_t speedRF, int8_t speedLB, int8_t speedRB) {
    motorLF.setSpeed(speedLF);
    motorRF.setSpeed(speedRF);
    motorLB.setSpeed(speedLB);
    motorRB.setSpeed(speedRB);
}

void Robot::updateSpeed() {
    controller_state_t state = controller.getState();
    int ly = state.ly;
    int lx = state.lx;
    int ry = state.ry;
    int rx = state.rx;
    if (abs(ry) > abs(rx) && ((ry > 0 && ly < 0) || (ry < 0 && ly > 0))) {
        // Разворот
        setSpeed(13 * ly, 13 * ry, 13 * ly, 13 * ry);
    } else {
        // Алгоритм Вани
        int speed1 = 7 * (ly + lx);
        int speed2 = 7 * (ly - lx);
        int rotate = 4 * rx;
        setSpeed(speed1 + rotate, speed2 - rotate, speed2 + rotate, speed1 - rotate);
    }
}
