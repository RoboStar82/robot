
#include "lidar.h"

Lidar lidar;

Lidar *getLidar() {
    return &lidar;
}

void lidarSetup() {
    lidar.getDeviceInfo();
    lidar.getDeviceHealth();
    xTaskCreatePinnedToCore(lidarBegin, "rplidar", 65536, NULL, 1, NULL, 0);
}

void lidarBegin(void *params) {
    delay(1000);
    lidar.begin();
    lidar.start();
    while (true) {
        if (lidar.isScan) {
            lidar.loop();
        } else {
            delay(1000);
        }
    }
}

void lidarLoop() {
    lidar.loop();
}
