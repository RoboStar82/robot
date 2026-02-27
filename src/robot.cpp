
#include "robot.h"

#include "controller.h"
#include "encoder.h"
#include "imu.h"
#include "lidar.h"
#include "motor.h"
#include "servo.h"

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

    if (state.ly == 0) {
        motor1.setSpeed(0);
        motor2.setSpeed(0);
    } else if (wheelDown) {
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
        servo1.setAngle(96);
        servo2.setAngle(86);
        wheelDown = false;
    } else if (state.dy < 0) {
        servo1.setAngle(86);
        servo2.setAngle(96);
        wheelDown = true;
    }
    if (state.dx > 0) {
        if (state.x || state.y) {
            servo8.setAngle(90);
        } else if (state.a || state.b) {
            servo8.setAngle(180);
        }
    } else {
        if (state.x) {
            servo4.setAngle(60);
        } else if (state.a) {
            servo4.setAngle(92);
        }
        if (state.y) {
            servo6.setAngle(60);
        } else if (state.b) {
            servo6.setAngle(90);
        }
    }
}

void Robot::updateCount() {
    controller_state_t state = controller.getState();
    if (state.start || state.back) {
        return;
    }
    bool updateCountLZ = false;
    bool updateCountRZ = false;
    bool updateCountRZ2 = false;
    if (state.lz > 0) {
        countLZ = min(countLZ + 1, 50);
        updateCountLZ = true;
    } else if (state.lz < 0) {
        countLZ = max(countLZ - 1, 0);
        updateCountLZ = true;
    }
    if (state.dx > 0) {
        if (state.rz > 0) {
            countRZ2 = min(countRZ2 + 1, 50);
            updateCountRZ2 = true;
        } else if (state.rz < 0) {
            countRZ2 = max(countRZ2 - 1, 0);
            updateCountRZ2 = true;
        }
    } else {
        if (state.rz > 0) {
            countRZ = min(countRZ + 1, 50);
            updateCountRZ = true;
        } else if (state.rz < 0) {
            countRZ = max(countRZ - 1, 0);
            updateCountRZ = true;
        }
    }
    if (updateCountLZ) {
        servo3.setAngle(180.0f - 180.0f / 50.0f * countLZ);
    }
    if (updateCountRZ) {
        servo5.setAngle(180.0f - 180.0f / 50.0f * countRZ);
    }
    if (updateCountRZ2) {
        servo7.setAngle(90.0f - 180.0f / 50.0f * countRZ2);
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
    servo1.begin(96);
    servo2.begin(86);
    servo3.setMaxAngle(360);
    servo3.begin(180);
    servo4.begin(91);
    servo5.begin(180);
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
    road_object_t objects[4];
    int objectCount = 0;
    int signWidth = -1;
    int signIndex = -1;
#if ROBOT_HAS_LIDAR
    // Сканируем объекты перед роботом
    for (int n = 0; n < 10; n++) {
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
        log_i("Lidar: Sign: angle=%d-%d, distance=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
    }
    for (int n = 0; n < objectCount; n++) {
        if (n != signIndex) {
            // Нашли столбы
            road_object_t object = objects[n];
            log_i("Lidar: Pillar: angle=%d-%d, distance=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
        }
    }
#endif
    int startAxisX = 0;
    int startAxisY = 0;
    int startAxisZ = 0;
#if ROBOT_HAS_IMU
    // Начальные позиции гироскопа
    startAxisX = imu.getAxisX();
    startAxisY = imu.getAxisY();
    startAxisZ = imu.getAxisZ();
    log_i("Gyroscope: x=%d, y=%d, z=%d", startAxisX, startAxisY, startAxisZ);
#endif
    // Имитация нажатий на кнопки
    controller_state_t state = controller.getState();
    // Едем вперед и рога вниз
    state.ly = 5;
    state.lz = 1;
    state.rz = 1;
    controller.setState(state);
    vTaskDelay(2000);
    // Рога опущены но мы едем дальше
    state.lz = 0;
    state.rz = 0;
    controller.setState(state);
    vTaskDelay(600);
    // Остановка и кидаем палки
    state.ly = 0;
    state.x = 1;
    state.y = 1;
    controller.setState(state);
    vTaskDelay(400);
    // Едем назад и рога вверх
    state.ly = -5;
    state.lz = -1;
    state.rz = -1;
    controller.setState(state);
    vTaskDelay(200);
    // Рога подняты но мы едем дальше
    state.lz = 0;
    state.rz = 0;
    vTaskDelay(2200);
    // Остановка
    state.ly = 0;
    controller.setState(state);
    // vTaskDelay(1000);
    startAxisZ = imu.getAxisZ();
    log_i("Gyroscope: start z=%d", startAxisZ);
    // Поворачиваем направо
    state.rx = 4;
    controller.setState(state);
    vTaskDelay(3000);
    state.rx = 0;
    controller.setState(state);
    // vTaskDelay(2000);
    log_i("Gyroscope: deltaZ=%d", imu.getAxisZ() - startAxisZ);
    int distance0 = 0;
    int distance1 = 0;
    // Смотрим где горка
    lidar.scanRamp(distance0, distance1);
    log_i("Lidar: Ramp: %d-%d", distance0, distance1);
    for (int n = 0; n < 10; n ++) {
        if (abs(distance1 - distance0) > 10) {
            if (distance1 < distance0) {
                state.rx = 3;
            } else {
                state.rx = -3;
            }
            // Поворачиваем параллельно горке
            controller.setState(state);
            vTaskDelay(1000 - 50 * n);
            lidar.scanRamp(distance0, distance1);
            log_i("Lidar: Ramp: %d-%d", distance0, distance1);
        } else {
            break;
        }
    }
    log_i("Lidar: Ramp: %d-%d", distance0, distance1);
    // Прекращаем вертеться и опускаем колеса
    state.rx = 0;
    state.dy = -1;
    controller.setState(state);
    vTaskDelay(100);
    // Поехали на горку
    state.dy = 0;
    state.ly = 7;
    controller.setState(state);
    vTaskDelay(1000);
    state.ly = 0;
    controller.setState(state);
    // Приехали
    autoEnd();
    vTaskDelay(200);
    // Фиксируемся
    motor1.setSpeed(10);
    motor2.setSpeed(10);
    autoStartedTask = nullptr;
    vTaskDelete(NULL);
}

void Robot::autoTask(void* arg) {
    robot.autoTask();
}