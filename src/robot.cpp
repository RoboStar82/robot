
#include "robot.h"

#include "controller.h"
#include "encoder.h"
#include "imu.h"
#include "lidar.h"
#include "motor.h"
#include "servo.h"
#include "led.h" // Не забудь убрать

Robot robot;

Robot::Robot() {}

Robot::~Robot() {}

void Robot::begin() {
    if (!startedTask) {
        xTaskCreate(task, "robot_task", 4096, NULL, 1, &startedTask);
    }
}

void Robot::setSpeed(int speedLF, int speedRF, int speedLB, int speedRB) {
    motorLF.setSpeed(speedLF);
    motorRF.setSpeed(speedRF);
    motorLB.setSpeed(speedLB);
    motorRB.setSpeed(speedRB);
}

void Robot::autoStart() {
    if (!autoStartedTask) {
        xTaskCreate(autoTask, "robot_auto_task", 4096, NULL, 1, &autoStartedTask);
    }
}

void Robot::autoStop() {
    if (autoStartedTask) {
        log_i("Robot: auto stop");
        vTaskDelete(autoStartedTask);
        autoStartedTask = nullptr;
        autoEnd();
    }
}

void Robot::autoEnd() {
    log_i("Robot: auto end");
#if ROBOT_HAS_LIDAR
    lidar.stop();
#endif
    controller_state_t state = controller.getState();
    state.lx = 0;
    state.ly = 0;
    state.rx = 0;
    state.ry = 0;
    controller.setState(state);
}

void Robot::updateSpeed() {
    controller_state_t state = controller.getState();

    int ly = (int)36 * state.ly;
    int lx = (int)36 * state.lx;
    int ry = (int)-30 * state.ry;
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
    if (state.rz == 1) {
        motor3.setSpeed(254);
        mastBakc = true;
    } else if(state.rz == -1) {
        motor3.setSpeed(-254);
        mastBakc = false;
    } else {
        motor3.setSpeed(0);
    }

    if (wheelDown) {
        motor1.setSpeed(ly + (rx >> 2));
        motor2.setSpeed(ly - (rx >> 2));
    } else {
        motor1.setSpeed(0);
        motor2.setSpeed(0);
    }
}

void Robot::updateServo() {
    updateCount();
    controller_state_t state = controller.getState();
    if (state.start || state.back) {
        return;
    }
    if (state.dy > 0) {
        servo1.setAngle(102);
        servo2.setAngle(82);
        wheelDown = false;
    } else if (state.dy < 0) {
        servo1.setAngle(84);
        servo2.setAngle(100);
        wheelDown = true;
    }
    if (state.a) {
        servo5.setAngle(110);
    } else if (state.x) {
        servo5.setAngle(43);
    }
}

void Robot::updateCount() {
    controller_state_t state = controller.getState();
    if (state.start || state.back) {
        return;
    }

    bool updateCountDX = false;
    bool updateCountLZ = false;

    if (state.dx == 1) {
        countDX = min(countDX + 2 * mastBakc + 1, 116);
        updateCountDX = true;
    } else if (state.dx == -1) {
        countDX = max(countDX - 2 * mastBakc + 1, -64);
        updateCountDX = true;
    }
    if (state.lz == 1) {
        countLZ = min(countLZ + 2, 42);
        updateCountLZ = true;
    } else if (state.lz == -1) {
        countLZ = max(countLZ - 2, -42);
        updateCountLZ = true;
    }
    if (updateCountDX) {
        servo3.setAngle(232.0f - 180.0f / 90.0f * countDX);
    }
    if (updateCountLZ) {
        servo4.setAngle(90.0f - 180.0f / 90.0f * countLZ);
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

void Robot::needUpdateAutoStart() {
    robot_update_t value = ROBOT_UPDATE_START;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateAutoStop() {
    robot_update_t value = ROBOT_UPDATE_STOP;
    xQueueSend(needQueue, &value, 0);
}

void Robot::stop() {
    setSpeed(0, 0, 0, 0);
}

void Robot::task() {
    motorLF.begin();
    motorRF.begin();
    motorLB.begin();
    motorRB.begin();
    motor1.begin();
    motor2.begin();
    motor3.begin();
    servo1.begin(102);
    servo2.begin(82);
    servo3.setMaxAngle(360);
    servo3.begin(232);
    servo4.begin();
    servo5.begin(43);
    servo6.begin();
    servo7.begin();
    servo8.begin();
#if ROBOT_HAS_LIDAR
    lidar.start();
#endif
    while (true) {
        robot_update_t update;
        if (xQueueReceive(needQueue, &update, 50)) {
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
    log_i("Robot: auto start");
    // Сканируем объекты перед роботом
    road_object_t objects[4];
    int signWidth = -1;
    int signIndex = -1;
    float roadWorkAngle;
    int countRoadWorkAngle;
    int randomiseState = -1;
    int objectCount = 0;
    controller_state_t state;
#if ROBOT_HAS_LIDAR
    for (int n = 0; n < 10; n++) {
        objectCount = 0;
        lidar.scanRoadObjects(objects, objectCount, 4);
        vTaskDelay(100);
    }
    // Самый широкий объект знак
    for (int n = 0; n < objectCount; n++) {
        road_object_t object = objects[n];
        if (signWidth < object.width) {
            signWidth = object.width;
            signIndex = n;
        }
    }
    if (signIndex >= 0) {
        // Нашли знак
        road_object_t object = objects[signIndex];
        log_i("Sign: ang=%d-%d, disе=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
    }
    for (int n = 0; n < objectCount; n++) {
        if (n != signIndex) {
            // Нашли столбы
            road_object_t object = objects[n];
            log_i("Pillar: ang=%d-%d, dist=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
        }
    }
    if (objectCount == 3 || signIndex == 0) {
        randomiseState = signIndex;
    } else if (signIndex == 3) {
        randomiseState = 2;
    } else {
        if (objects[signIndex].angle0 - objects[signIndex - 1].angle1 > objects[signIndex + 1].angle0 - objects[signIndex].angle1) {
            randomiseState = signIndex;
        } else {
            randomiseState = --signIndex;
        }
    }
    state = controller.getState();
    switch (randomiseState) {
    case 0:
        state.x = 1;
        break;
    case 1:
        state.y = 1;
        break;
    case 2:
        state.b = 1;
        break;
    default:
        state.a = 1;
        break;
    }
    controller.setState(state);
#endif

    if (randomiseState == 2) {
        roadWorkAngle = (float)(objects[1].angle0 + objects[1].angle1) / 2; // работает как для 3 так и 4 объектов
    } else {
        roadWorkAngle = (float)(objects[-1].angle0 + objects[-1].angle1) / 2; // работает как для 3 так и 4 объектов
    }
    countRoadWorkAngle = (232.0f - roadWorkAngle) * 90.0f / 180.0f;

    if (countDX < countRoadWorkAngle) {
        for (countDX; countDX <= countRoadWorkAngle; countDX++) {
            servo3.setAngle(232.0f - 180.0f / 90.0f * countDX);
            vTaskDelay(50);
        }
    }  else if (countDX > countRoadWorkAngle) {
        for (countDX; countDX >= countRoadWorkAngle; countDX--) {
            servo3.setAngle(232.0f - 180.0f / 90.0f * countDX);
            vTaskDelay(50);
        }
    }
    state.rz = -1;
    controller.setState(state);
    vTaskDelay(objects[signIndex].distance * 20);
    state.rz = 1;
    state.x = 1;
    controller.setState(state);
    vTaskDelay(objects[signIndex].distance * 20);
    // Поехали на горку
    state.rz = 0;
    state.ly = -7;
    controller.setState(state);
    vTaskDelay(6000);
    state.ly = 0;
    controller.setState(state);
    // Приехали
    motor1.setSpeed(70);
    motor2.setSpeed(70);
    vTaskDelay(200);
    autoEnd();
    // Фиксируемся
    motor1.setSpeed(70);
    motor2.setSpeed(70);
    vTaskDelay(200);
    autoStartedTask = nullptr;
    vTaskDelete(NULL);
}

void Robot::autoTask(void* arg) {
    robot.autoTask();
}