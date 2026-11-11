
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

typedef enum {
    ROBOT_UPDATE_STOP = 0,
    ROBOT_UPDATE_START = 1,
    ROBOT_UPDATE_SPEED = 2,
    ROBOT_UPDATE_SERVO = 3,
} RobotUpdate_t;

class Robot {
   public:
    Robot();
    virtual ~Robot() = default;

    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void autoStart();
    void autoStop();
    void autoEnd();
    void updateSpeed();
    void updateServo();
    void updateCount();

    void needUpdateAutoStart();
    void needUpdateAutoStop();
    void needUpdateSpeed();
    void needUpdateServo();

    void stop();

    void task();

    static void task(void* arg);

    void autoTask();

    static void autoTask(void* arg);

   protected:
    TaskHandle_t taskHandle = nullptr;
    TaskHandle_t autoTaskHandle = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(RobotUpdate_t));
};

extern Robot robot;
