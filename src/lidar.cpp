
#include "lidar.h"

Lidar lidar;

Lidar *getLidar() {
    return &lidar;
}

void lidarSetup() {
    xTaskCreatePinnedToCore(lidarBegin, "rplidar", 65536, NULL, 1, NULL, 0);
}

void lidarBegin(void *params) {
    delay(1000);
    lidar.begin();
    while (true) {
        if (lidar.started) {
            lidar.loop();
        } else {
            delay(1000);
        }
    }
}

void lidarLoop() {
    lidar.loop();
}
