
#include "robot.h"

void setup() {
    Serial.begin(115200);
    otaFinish();
    bleSetup();
#if ROBOT_HAS_BMX
    bmxSetup();
#endif
#if ROBOT_HAS_LIDAR
    lidarSetup();
#endif
    robotSetup();
}

void loop() {
    delay(1000);
}
