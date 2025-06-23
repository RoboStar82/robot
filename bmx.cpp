
#include "bmx.h"

iarduino_Position_BMX055 sensor(BMX);

float bmxCoefficients[3] = { 135.00, 105.00, 0.00 };

iarduino_Position_BMX055 *getBMX() {
    return &sensor;
}

void bmxSetup() {
    xTaskCreatePinnedToCore(bmxBegin, "bmx055", 8192, NULL, 1, NULL, 0);
}

void bmxBegin(void *params) {
    sensor.begin(&Wire, true);
    sensor.setFastOffset();
    sensor.setFastOffset(bmxCoefficients);
    delay(1000);
    while (true) {
        bmxLoop();
    }
}

void bmxCalibrate() {
    Serial.println("V: compass: calibrate");
    unsigned long i = millis();
    while ((millis() - i) < 30000) {
        sensor.setFastOffset();
    }
    sensor.getFastOffset(bmxCoefficients);
    Serial.print("V: compass: float bmxCoefficients[3] = { ");
    Serial.print(bmxCoefficients[0]); Serial.print(", ");
    Serial.print(bmxCoefficients[1]); Serial.print(", ");
    Serial.print(bmxCoefficients[2]); Serial.println(" }");
}

void bmxLoop() {
    sensor.read();
}
