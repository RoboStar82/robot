
#include "imu.h"

IMU imu;

IMU::IMU() {
#if ROBOT_HAS_IMU
    bmx = new iarduino_Position_BMX055(BMX);
#endif
}

IMU::~IMU() {}

void IMU::begin() {
    if (bmx) {
        if (bmx->begin(&Wire)) {
            log_i("Gyroscope: connected");
            xTaskCreatePinnedToCore(task, "imu_task", 4096, NULL, 1, &startedTask, 0);
        } else {
            log_i("Gyroscope: not connected");
        }
    }
}

int IMU::getAxisX() {
    if (bmx) {
        return bmx->axisX;
    } else {
        return 0;
    }
}

int IMU::getAxisY() {
    if (bmx) {
        return bmx->axisY;
    } else {
        return 0;
    }
}

int IMU::getAxisZ() {
    if (bmx) {
        return bmx->axisZ;
    } else {
        return 0;
    }
}

void IMU::calibrate(int time) {
    float coefficients[3] = { 0, 0, 0 };
    for (int n = 0; n < 10; n ++) {
        vTaskDelay(1000);
        log_i("Gyroscope: calibrate");
        unsigned long i = millis();
        while ((millis() - i) < time) {
            bmx->setFastOffset(BMM);
            vTaskDelay(1);
        }
        bmx->getFastOffset(coefficients);
        log_i("float coefficients[3] = { %f, %f, %f }", coefficients[0], coefficients[1], coefficients[2]);
    }
}

void IMU::task() {
    bmx->setScale(BMA_8G);
    bmx->setScale(BMG_500DPS);
    bmx->setScale(BMM_HIGH);
    bmx->setBandwidths(BMA_63Hz);
    bmx->setBandwidths(BMG_64Hz);
    bmx->setBandwidths(BMM_20Hz);
    bmx->setFastOffset(BMA);
    bmx->setFastOffset(BMG);
    // calibrate();
    float coefficients[3] = { -179.375000, -116.875000, -81.875000 };
    bmx->setFastOffset(coefficients);
    int count = 0;
    int debug = 0;
    while (true) {
        if (!bmx->read()) {
            vTaskDelay(1);
            continue;
        }
        /*
        if (++count > 10) {
            count = 0;
            if (++debug > 10) {
                debug = 0;
                log_i("Gyroscope: x=%d, y=%d, z=%d", (int)bmx->axisX, (int)bmx->axisY, (int)bmx->axisZ);
            }
        }
        */
    }
}

void IMU::task(void* arg) {
    imu.task();
}
