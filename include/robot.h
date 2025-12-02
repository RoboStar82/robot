
#pragma once

#include <Arduino.h>

typedef enum {
    ROBOT_UPDATE_SPEED = 1,
    ROBOT_UPDATE_SERVO = 2,
} robot_update_t;

class Robot {
   public:
    Robot();
    ~Robot();

    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void updateSpeed();
    void updateServo();

    void needUpdateSpeed();
    void needUpdateServo();

    void task();

    static void task(void* arg);

   protected:
    bool started = false;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(robot_update_t));
    int raise = 0;
};

extern Robot robot;
