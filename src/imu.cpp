
#include "imu.h"

IMU imu;

IMU::IMU() {
#if ROBOT_HAS_IMU
    bmx = new iarduino_Position_BMX055(BMX);
#endif
}

IMU::~IMU() {}

void IMU::begin() {
    if (bmx != nullptr) {
        bmx->begin(&Wire, true);
        bmx->setFastOffset();
        bmx->setFastOffset(coefficients);
        xTaskCreatePinnedToCore(task, "imu_task", 4096, NULL, 1, NULL, 0);
    }
}

void IMU::calibrate(int time) {
    log_i("...");
    unsigned long i = millis();
    while ((millis() - i) < time) {
        bmx->setFastOffset();
    }
    bmx->getFastOffset(coefficients);
    log_i("float coefficients[3] = { %f, %f, %f }", coefficients[0], coefficients[1], coefficients[2]);
}

void IMU::task() {
    while (true) {
        bmx->read();
    }
}

void IMU::task(void* arg) {
    imu.task();
}
