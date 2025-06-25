
#include "robot.h"

void setup() {
    Serial.begin(115200);
    bleSetup();
    bmxSetup();
    lidarSetup();
    robotSetup();
}

void loop() {
    delay(1000);
}
