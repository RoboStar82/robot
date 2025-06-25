
#include "bmx.h"

iarduino_Position_BMX055 *bmx = new iarduino_Position_BMX055(BMX);

float bmxCoefficients[3] = {135.00, 105.00, 0.00};

iarduino_Position_BMX055 *getBMX() {
    return bmx;
}

void bmxSetup() {
    xTaskCreatePinnedToCore(bmxBegin, "bmx055", 8192, NULL, 1, NULL, 0);
}

void bmxBegin(void *params) {
    bmx->begin(&Wire, true);
    bmx->setFastOffset();
    bmx->setFastOffset(bmxCoefficients);
    delay(1000);
    while (true) {
        bmxLoop();
    }
}

void bmxCalibrate(int time) {
    println("V: BMX: calibrate");
    unsigned long i = millis();
    while ((millis() - i) < time) {
        bmx->setFastOffset();
    }
    bmx->getFastOffset(bmxCoefficients);
    print("V: BMX: float bmxCoefficients[3] = { ");
    print(bmxCoefficients[0]);
    print(", ");
    print(bmxCoefficients[1]);
    print(", ");
    print(bmxCoefficients[2]);
    println(" }");
}

void bmxLoop() {
    bmx->read();
}
