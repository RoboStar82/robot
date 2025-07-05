
#include "robot.h"

void setup() {
    Serial.begin(115200);
    bleSetup();
#if 0
    bmxSetup();
#endif
#if 0
    lidarSetup();
#endif
    robotSetup();
}

void loop() {
    delay(1000);
}
