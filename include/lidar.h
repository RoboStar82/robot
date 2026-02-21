
#pragma once

#include <Arduino.h>
#include <freertos/task.h>

#include <mutex>

#if ROBOT_HAS_LIDAR
#ifndef ROBOT_LIDAR_RX_PIN
#define ROBOT_LIDAR_RX_PIN 15
#endif
#ifndef ROBOT_LIDAR_TX_PIN
#define ROBOT_LIDAR_TX_PIN 16
#endif
#ifndef LidarSerial
#define LidarSerial Serial1
#endif
#ifndef LIDAR_DEBUG_TX
#define LIDAR_DEBUG_TX 0
#endif
#ifndef LIDAR_DEBUG_RX
#define LIDAR_DEBUG_RX 0
#endif
#endif

typedef enum {
    LIDAR_CONTROL_STOP = 0,
    LIDAR_CONTROL_START = 1,
} lidar_control_t;

// Точка
typedef struct {
    // Номер точки: 0..255
    uint8_t n = 0;
    // X
    int16_t x = 0;
    // Y
    int16_t y = 0;
} __attribute__((packed)) lidar_point_t;

class AnyLidar {
   public:
    // Когда начато сканирование
    unsigned long started = 0;
    // Что необходимо сделать в цикле сканирования
    lidar_control_t control = LIDAR_CONTROL_STOP;

    // Количество ошибок
    int errorCount = 0;

    // Точки
    lidar_point_t points[256];
    // 0..256
    int pointCount = 0;

    // Блокировка
    std::mutex lockData;

    // Синусы углов от 0 до 90: 0..4096
    uint16_t sinuses[91] = {0, 71, 143, 214, 286, 357, 428, 499, 570, 641, 711, 782, 852, 921, 991, 1060, 1129, 1198, 1266, 1334, 1401, 1468, 1534, 1600, 1666, 1731, 1796, 1860, 1923, 1986, 2048, 2110, 2171, 2231, 2290, 2349, 2408, 2465, 2522, 2578, 2633, 2687, 2741, 2793, 2845, 2896, 2946, 2996, 3044, 3091, 3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435, 3474, 3511, 3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798, 3824, 3849, 3873, 3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021, 4034, 4046, 4056, 4065, 4074, 4080, 4086, 4090, 4094, 4095, 4096};

    // Косинусы углов от 0 до 90: 0..4096
    uint16_t cosines[91] = {4096, 4095, 4094, 4090, 4086, 4080, 4074, 4065, 4056, 4046, 4034, 4021, 4006, 3991, 3974, 3956, 3937, 3917, 3896, 3873, 3849, 3824, 3798, 3770, 3742, 3712, 3681, 3650, 3617, 3582, 3547, 3511, 3474, 3435, 3396, 3355, 3314, 3271, 3228, 3183, 3138, 3091, 3044, 2996, 2946, 2896, 2845, 2793, 2741, 2687, 2633, 2578, 2522, 2465, 2408, 2349, 2290, 2231, 2171, 2110, 2048, 1986, 1923, 1860, 1796, 1731, 1666, 1600, 1534, 1468, 1401, 1334, 1266, 1198, 1129, 1060, 991, 921, 852, 782, 711, 641, 570, 499, 428, 357, 286, 214, 143, 71, 0};

    virtual bool reset() {
        errorCount = 0;
        pointCount = 0;
        return true;
    }

    virtual bool start() {
        started = millis();
        return true;
    }

    virtual bool stop() {
        started = 0;
        return true;
    }

    virtual bool scan(uint16_t &angle, uint16_t &distance, uint8_t &strength) {
        if (!started) {
            return false;
        }
        angle = 0;
        distance = 0;
        strength = 0;
        return true;
    }

    bool loop() {
        if (control == LIDAR_CONTROL_STOP) {
            // Сканирование начато, необходимо остановить
            stop();
            vTaskDelete(NULL);
            return false;
        } else if (!started) {
            // Сканирование не начато. Необходимо начать
            if (!start()) {
                return false;
            }
        }
        lidar_point_t points[256];
        int pointCount = 0;
        if (!scanLoop(points, pointCount)) {
            if (!pointCount && millis() - started > 3000) {
                errorCount++;
                if (errorCount > 9) {
                    log_e("Lidar: %d errors\n", errorCount);
                    stop();
                    delay(1000);
                    start();
                }
            }
            return false;
        } else {
            errorCount = 0;
        }
        lockData.lock();
        memcpy(this->points, points, sizeof(points));
        this->pointCount = pointCount;
        lockData.unlock();
        return true;
    }

    bool scanLoop(lidar_point_t points[], int &pointCount) {
        // 0..359
        uint16_t prevAngle = 0;
        for (int n = 0; n < 256; n++) {
            // 0..359
            uint16_t angle = 0;
            uint16_t angle1 = 0;
            uint16_t angle2 = 0;
            // 0..
            uint16_t distance = 0;
            uint16_t distance1 = 0;
            uint16_t distance2 = 0;
            // 0..
            uint8_t strength = 0;
            if (!scan(angle1, distance1, strength)) {
                return false;
            }
            if (!scan(angle2, distance2, strength)) {
                return false;
            }
            if (distance1 > distance2) {
                angle = angle1;
                distance = distance1;
            } else {
                angle = angle2;
                distance = distance2;
            }
            /*
             * Углы должны идти последовательно, чтобы вершины выпуклой оболочки тоже были последовательными
             * Это правило нарушается в начале вращения лидара: во время первого оборота углы меняются в случайной последовательности
             * Не продолжаем обработку точек, если углы не являются последовательными
             */
            if (n) {
                if (angle == prevAngle) {
                    continue;
                } else if (angle > prevAngle) {
                    if (angle - prevAngle > 3) {
                        // debug("E: lidar: unexpected angle %d after %d\n", angle, prevAngle);
                        return false;
                    }
                } else {
                    if (360 + angle - prevAngle > 3) {
                        // debug("E: lidar: unexpected angle %d after %d\n", angle, prevAngle);
                        return false;
                    }
                }
            }
            prevAngle = angle;
            if (!distance) {
                continue;
            }
            lidar_point_t point;
            // -4096..4096
            int angleSin = 0;
            // -4096..4096
            int angleCos = 0;
            if (angle <= 90) {
                angleSin = sinuses[angle];
                angleCos = cosines[angle];
            } else if (angle <= 180) {
                angleSin = cosines[angle - 90];
                angleCos = -sinuses[angle - 90];
            } else if (angle <= 270) {
                angleSin = -sinuses[angle - 180];
                angleCos = -cosines[angle - 180];
            } else {
                angleSin = -cosines[angle - 270];
                angleCos = sinuses[angle - 270];
            }
            point.n = pointCount;
            point.x = ((int)distance * angleCos) >> 12;
            point.y = ((int)distance * angleSin) >> 12;
            points[pointCount] = point;
            pointCount++;
            if (pointCount >= 256) {
                break;
            }
        }
        if (pointCount < 4) {
            // debug("E: lidar: unexpected point count %d\n", pointCount);
            return false;
        }
        return true;
    }
};

class RPLidar : public AnyLidar {
   public:
    void begin();

    bool sendCommand(uint8_t command, uint8_t *payload = nullptr, uint8_t size = 0);

    bool readHeader(uint8_t &type, uint8_t &subtype, uint32_t &length);

    bool skipAll();

    bool skipBytes(size_t length);

    bool getDeviceInfo();

    bool getDeviceHealth();

    bool getLidarConf(uint32_t conf);

    bool reset() override;

    bool start() override;

    bool stop() override;

    bool scan(uint16_t &angle, uint16_t &distance, uint8_t &strength) override;
};

class Lidar : public RPLidar {
};

extern Lidar lidar;
