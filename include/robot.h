
#pragma once

#include <Arduino.h>

typedef enum {
    ROBOT_UPDATE_STOP = 0,
    ROBOT_UPDATE_START = 1,
    ROBOT_UPDATE_SPEED = 2,
    ROBOT_UPDATE_SERVO = 3,
} robot_update_t;

class Robot {
   public:
    Robot();
    ~Robot();

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
    TaskHandle_t startedTask = nullptr;
    TaskHandle_t autoStartedTask = nullptr;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(robot_update_t));
    int countRZ = 0;
    int countDX = 0;
    bool wheelDown = false;
};

extern Robot robot;
