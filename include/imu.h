
#pragma once

#include <Arduino.h>
#include <iarduino_Position_BMX055.h>

class IMU {
   public:
    IMU();
    ~IMU();

    void begin();

    void calibrate(int time = 30000);

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
    float coefficients[3] = {135.00, 105.00, 0.00};
    iarduino_Position_BMX055* bmx = nullptr;
};

extern IMU imu;
