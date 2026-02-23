
#pragma once

#include <Arduino.h>
#include <iarduino_Position_BMX055.h>

class IMU {
   public:
    IMU();
    ~IMU();

    void begin();

    int getAxisX();
    int getAxisY();
    int getAxisZ();

    void calibrate(int time = 10000);

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
    iarduino_Position_BMX055* bmx = nullptr;
};

extern IMU imu;
