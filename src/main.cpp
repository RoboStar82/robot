
#include "robot.h"

void setup() {
    Serial.begin(115200);
    bleSetup();
#if ROBOT_HAS_BMX055
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
