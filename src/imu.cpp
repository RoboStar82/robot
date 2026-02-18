
#include "imu.h"

IMU imu;

IMU::IMU() {
#if ROBOT_HAS_IMU
    bmx = new iarduino_Position_BMX055(BMG);
#endif
}

IMU::~IMU() {}

void IMU::begin() {
    if (bmx != nullptr) {
        started = bmx->begin(&Wire, true);
        if (started) {
            log_i("Gyroscope: connected");
            bmx->setFastOffset();
            bmx->setFastOffset(coefficients);
            xTaskCreatePinnedToCore(task, "imu_task", 4096, NULL, 1, NULL, 0);
        } else {
            log_i("Gyroscope: not connected");
        }
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
    int count = 0;
    int debug = 0;
    while (true) {
        bmx->read();
        if (++count > 1024) {
            count = 0;
            if (++debug > 1) {
                debug = 0;
                log_i("Gyroscope: %d", (int)bmx->axisZ);
            }
        }
    }
}

void IMU::task(void* arg) {
    imu.task();
}
