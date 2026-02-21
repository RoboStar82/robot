
#pragma once

#include <Arduino.h>

typedef enum {
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

    void updateStart();
    void updateSpeed();
    void updateServo();

    void needUpdateStart();
    void needUpdateSpeed();
    void needUpdateServo();

    void updateCount();

    void task();

    static void task(void* arg);

   protected:
    bool started = false;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(robot_update_t));
    int countX = 0;
    int countY = 0;
    int countLZ = 0;
    int countRZ = 0;
};

extern Robot robot;
