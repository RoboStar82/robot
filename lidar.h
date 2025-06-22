
#pragma once

#include <Arduino.h>
#include <freertos/task.h>
#include <mutex>

#define lidarSerial Serial1
#define lidarRxPin 15
#define lidarTxPin 16

#define lidarPointDebug 0

// Точка
typedef struct lidar_point_t {
    // Номер точки: 0..359
    uint16_t n = 0;
    // X
    int16_t x = 0;
    // Y
    int16_t y = 0;
    // Является ли точка вершиной выпуклой оболочки
    bool isEdge = false;
} __attribute__((packed)) lidar_point_t;

class AnyLidar {

public:

    // Идёт ли сканирование
    bool isScan = false;

    // Точки
    lidar_point_t points[360];
    // 0..360
    uint16_t pointCount = 0;
    // 0..360
    uint16_t edgeCount = 0;

    // Угол поворота стен 0..89
    uint8_t angle = 0;
    // Точки на стенах
    uint16_t walls[4] = { 0, 0, 0, 0 };

    // Блокировка
    std::mutex lockData;

    // Синусы углов от 0 до 90: 0..4096
    uint16_t sinuses[91] = { 0, 71, 143, 214, 286, 357, 428, 499, 570, 641, 711, 782, 852, 921, 991, 1060, 1129, 1198, 1266, 1334, 1401, 1468, 1534, 1600, 1666, 1731, 1796, 1860, 1923, 1986, 2048, 2110, 2171, 2231, 2290, 2349, 2408, 2465, 2522, 2578, 2633, 2687, 2741, 2793, 2845, 2896, 2946, 2996, 3044, 3091, 3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435, 3474, 3511, 3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798, 3824, 3849, 3873, 3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021, 4034, 4046, 4056, 4065, 4074, 4080, 4086, 4090, 4094, 4095, 4096 };

    // Косинусы углов от 0 до 90: 0..4096
    uint16_t cosines[91] = { 4096, 4095, 4094, 4090, 4086, 4080, 4074, 4065, 4056, 4046, 4034, 4021, 4006, 3991, 3974, 3956, 3937, 3917, 3896, 3873, 3849, 3824, 3798, 3770, 3742, 3712, 3681, 3650, 3617, 3582, 3547, 3511, 3474, 3435, 3396, 3355, 3314, 3271, 3228, 3183, 3138, 3091, 3044, 2996, 2946, 2896, 2845, 2793, 2741, 2687, 2633, 2578, 2522, 2465, 2408, 2349, 2290, 2231, 2171, 2110, 2048, 1986, 1923, 1860, 1796, 1731, 1666, 1600, 1534, 1468, 1401, 1334, 1266, 1198, 1129, 1060, 991, 921, 852, 782, 711, 641, 570, 499, 428, 357, 286, 214, 143, 71, 0 };

    void begin() {
        return;
    }

    bool reset() {
        pointCount = 0;
        edgeCount = 0;
        angle = 0;
        walls[0] = 0;
        walls[1] = 0;
        walls[2] = 0;
        walls[3] = 0;
        return true;
    }

    bool start() {
        isScan = true;
        return true;
    }

    bool stop() {
        isScan = false;
        return true;
    }

    virtual bool scan(uint16_t &angle, uint16_t &distance, uint8_t &strength) {
        if (!isScan) {
            return false;
        }
        angle = pointCount < 360 ? pointCount : 0;
        distance = 0;
        strength = 0;
        return true;
    }

    bool loop() {
        if (!isScan) {
            return false;
        }
        lockData.lock();
        if (!scanLoop()) {
            lockData.unlock();
            return false;
        }
        if (!edgeLoop()) {
            lockData.unlock();
            return false;
        }
        if (!angleLoop()) {
            lockData.unlock();
            return false;
        }
        if (!wallLoop()) {
            lockData.unlock();
            return false;
        }
        lockData.unlock();
        return true;
    }

    void addPoint(uint16_t angle, uint16_t distance) {
        lidar_point_t point;
        // -4096..4096
        int16_t angleSin = 0;
        // -4096..4096
        int16_t angleCos = 0;
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
        point.x = (distance * angleCos) >> 12;
        point.y = (distance * angleSin) >> 12;
        points[pointCount] = point;
        pointCount ++;
    }

    bool scanLoop() {
        // 0..360
        pointCount = 0;
        // 0..360
        edgeCount = 0;
        // 0..359
        uint16_t prevAngle = 0;
        for (uint16_t n = 0; n < 512; n ++) {
            // 0..359
            uint16_t angle = 0;
            // 0..
            uint16_t distance = 0;
            // 0..
            uint8_t strength = 0;
            if (!scan(angle, distance, strength)) {
                return false;
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
                    if (angle - prevAngle > 1) {
                        return false;
                    }
                } else {
                    if (360 + angle - prevAngle > 1) {
                        return false;
                    }
                }
            }
            prevAngle = angle;
            if (!distance) {
                continue;
            }
            addPoint(angle, distance);
            if (pointCount >= 360) {
                break;
            }
        }
        if (pointCount < 4) {
            return false;
        }
        return true;
    }

    bool edgeLoop() {
        // 0..359
        uint16_t minN = 0;
        // 0..359
        uint16_t maxN = 0;
        for (uint16_t n = 0; n < pointCount; n ++) {
            if (points[n].x < points[minN].x) {
                minN = n;
            }
            if (points[n].x > points[maxN].x) {
                maxN = n;
            }
        }
        quickHull(points, pointCount, points[minN], points[maxN], 1);
        quickHull(points, pointCount, points[minN], points[maxN], -1);
        for (uint16_t n = 0; n < pointCount; n ++) {
            if (points[n].isEdge) {
                edgeCount ++;
            }
        }
        if (edgeCount < 3) {
            return false;
        }
        return true;
    }

    bool angleLoop() {
        // 0..359
        uint16_t prevN = 0;
        for (uint16_t n = pointCount - 1; n > 0; n --) {
            if (points[n].isEdge) {
                prevN = n;
                break;
            }
        }
        int32_t sumX = 0;
        int32_t sumY = 0;
        for (uint16_t n = 0; n < pointCount; n ++) {
            if (points[n].isEdge) {
                int16_t dx = points[n].x - points[prevN].x;
                int16_t dy = points[n].y - points[prevN].y;
                // 0..359
                uint16_t angle = ((uint16_t)(360 + atan2(dy, dx) * 180 / PI) << 2) % 360;
                // -4096..4096
                int16_t angleSin = 0;
                // -4096..4096
                int16_t angleCos = 0;
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
                uint32_t length = sqrt(dy * dy + dx * dx);
                sumX += (length * angleCos) >> 12;
                sumY += (length * angleSin) >> 12;
                prevN = n;
            }
        }
        if (sumX == 0 && sumY == 0) {
            angle = 0;
            return false;
        }
        angle = ((uint16_t)(360 + atan2(sumY, sumX) * 180 / PI) % 360) >> 2;
        return true;
    }

    bool wallLoop() {
        int16_t angleSin = sinuses[angle];
        int16_t angleCos = cosines[angle];
        int16_t maxDX = 0;
        int16_t maxDY = 0;
        int16_t minDX = 0;
        int16_t minDY = 0;
        uint16_t maxNX = 0;
        uint16_t maxNY = 0;
        uint16_t minNX = 0;
        uint16_t minNY = 0;
        for (uint16_t n = 0; n < pointCount; n ++) {
            if (points[n].isEdge) {
                int16_t dy = (points[n].y * angleCos - points[n].x * angleSin) >> 12;
                int16_t dx = (points[n].x * angleCos + points[n].y * angleSin) >> 12;
                if (minDX > dx) {
                    minDX = dx;
                    minNX = n;
                }
                if (maxDX < dx) {
                    maxDX = dx;
                    maxNX = n;
                }
                if (minDY > dy) {
                    minDY = dy;
                    minNY = n;
                }
                if (maxDY < dy) {
                    maxDY = dy;
                    maxNY = n;
                }
            }
        }
        walls[0] = maxNY;
        walls[1] = minNX;
        walls[2] = minNY;
        walls[3] = maxNX;
        return true;
    }

    void copyLoop(uint8_t *data, uint16_t &length) {
        lockData.lock();
        uint16_t index = 0;
        data[index] = angle;
        index ++;
        uint16_t resultWallIndex = index;
        uint8_t resultWallCount = 0;
        data[index] = resultWallCount;
        index ++;
        for (uint8_t i = 0; i < 4; i ++) {
            uint16_t n = walls[i];
            if (n < pointCount) {
                resultWallCount ++;
                data[index] = points[n].x & 0xff;
                index ++;
                data[index] = points[n].x >> 8;
                index ++;
                data[index] = points[n].y & 0xff;
                index ++;
                data[index] = points[n].y >> 8;
                index ++;
            }
        }
        data[resultWallIndex] = resultWallCount;
        uint16_t resultEdgeIndex = index;
        uint8_t resultEdgeCount = edgeCount < 64 ? edgeCount : 64;
        data[index] = resultEdgeCount;
        index ++;
        if (resultEdgeCount > 0) {
            uint16_t edgeN = 0;
            uint8_t nth = edgeCount > resultEdgeCount ? 1 + (edgeCount - 1) / resultEdgeCount : 0;
            resultEdgeCount = 0;
            for (uint16_t n = 0; n < pointCount; n ++) {
                if (points[n].isEdge) {
                    if (!nth || !(edgeN % nth)) {
                        resultEdgeCount ++;
                        data[index] = points[n].x & 0xff;
                        index ++;
                        data[index] = points[n].x >> 8;
                        index ++;
                        data[index] = points[n].y & 0xff;
                        index ++;
                        data[index] = points[n].y >> 8;
                        index ++;
                    }
                    edgeN ++;
                }
            }
            data[resultEdgeIndex] = resultEdgeCount;
        }
        uint16_t resultPointIndex = index;
        uint8_t resultPointCount = (pointCount - edgeCount < 64 - resultEdgeCount) ? pointCount - edgeCount : 64 - resultEdgeCount;
        data[index] = resultPointCount;
        index ++;
        if (resultPointCount > 0) {
            uint16_t pointN = 0;
            uint8_t nth = pointCount - edgeCount > resultPointCount ? 1 + (pointCount - edgeCount - 1) / resultPointCount : 0;
            resultPointCount = 0;
            for (uint16_t n = 0; n < pointCount; n ++) {
                if (!points[n].isEdge) {
                    if (!nth || !(pointN % nth)) {
                        resultPointCount ++;
                        data[index] = points[n].x & 0xff;
                        index ++;
                        data[index] = points[n].x >> 8;
                        index ++;
                        data[index] = points[n].y & 0xff;
                        index ++;
                        data[index] = points[n].y >> 8;
                        index ++;
                    }
                    pointN ++;
                }
            }
            data[resultPointIndex] = resultPointCount;
        }
        length = index;
        lockData.unlock();
    }

    void printPoints() {
        lockData.lock();
        uint8_t angle = this->angle;
        uint16_t pointCount = this->pointCount;
        uint16_t edgeCount = this->edgeCount;
        uint8_t wallCount = 0;
        lidar_point_t points[360];
        uint16_t walls[4];
        for (uint16_t n = 0; n < pointCount; n ++) {
            points[n] = this->points[n];
        }
        for (uint8_t i = 0; i < 4; i ++) {
            walls[i] = this->walls[i];
        }
        lockData.unlock();

        for (uint8_t i = 0; i < 4; i ++) {
            uint16_t n = walls[i];
            if (n < pointCount) {
                wallCount ++;
            }
        }
        Serial.printf("V: lidar: angle: %d\n", angle);
        Serial.printf("V: lidar: point count: %d\n", pointCount);
        Serial.printf("V: lidar: edge count: %d\n", edgeCount);
        Serial.printf("V: lidar: wall count: %d\n", wallCount);
        for (uint8_t i = 0; i < 4; i ++) {
            uint16_t n = walls[i];
            if (n < pointCount) {
                lidar_point_t point = points[n];
                Serial.printf("V: lidar: wall: n=%d; x=%d; y=%d; isEdge=%d\n", point.n, point.x, point.y, point.isEdge);
            }
        }
        for (uint16_t n = 0; n < pointCount; n ++) {
            lidar_point_t point = points[n];
            if (point.isEdge) {
                Serial.printf("V: lidar: edge: n=%d; x=%d; y=%d; isEdge=%d\n", point.n, point.x, point.y, point.isEdge);
            }
        }
        for (uint16_t n = 0; n < pointCount; n ++) {
            lidar_point_t point = points[n];
            if (!point.isEdge) {
                Serial.printf("V: lidar: point: n=%d; x=%d; y=%d; isEdge=%d\n", point.n, point.x, point.y, point.isEdge);
            }
        }
    }

protected:

    int8_t lineSide(lidar_point_t a, lidar_point_t b, lidar_point_t p) {
        int32_t value = (p.y - a.y) * (b.x - a.x) - (p.x - a.x) * (b.y - a.y);
        if (value == 0) {
            return 0;
        } else if (value > 0) {
            return 1;
        } else if (value < 0) {
            return -1;
        } else {
            return 0;
        }
    }

    uint32_t lineDistance(lidar_point_t a, lidar_point_t b, lidar_point_t p) {
        return abs((p.y - a.y) * (b.x - a.x) - (p.x - a.x) * (b.y - a.y));
    }

    void quickHull(lidar_point_t points[], uint16_t pointCount, lidar_point_t a, lidar_point_t b, int8_t side) {
        bool hasMaxDistance = false;
        uint16_t maxDistanceN = 0;
        uint32_t maxDistance = 0;
        for (uint16_t n = 0; n < pointCount; n ++) {
            int8_t currentSide = lineSide(a, b, points[n]);
            uint32_t distance = lineDistance(a, b, points[n]);
            if (currentSide == side && distance > maxDistance) {
                hasMaxDistance = true;
                maxDistanceN = n;
                maxDistance = distance;
            }
        }
        if (!hasMaxDistance) {
            points[a.n].isEdge = true;
            points[b.n].isEdge = true;
            return;
        }
        quickHull(points, pointCount, points[maxDistanceN], a, -lineSide(points[maxDistanceN], a, b));
        quickHull(points, pointCount, points[maxDistanceN], b, -lineSide(points[maxDistanceN], b, a));
    }

};

class RPLidar : public AnyLidar {

public:

    // Включена ли отладка
    bool debugRx = false;
    bool debugTx = false;

    void begin() {
        AnyLidar::begin();
        pinMode(lidarRxPin, INPUT);
        pinMode(lidarTxPin, OUTPUT);
        lidarSerial.setTimeout(100);
        lidarSerial.begin(460800);
    }

    bool sendCommand(uint8_t command, uint8_t *payload = nullptr, uint8_t size = 0) {
        uint8_t checksum = 0;
        if (lidarSerial.write(0xa5) < 1) {
            Serial.println("E: lidar: send byte (1) 0xa5");
            return false;
        }
        checksum ^= 0xa5;
        if (lidarSerial.write(command) < 1) {
            Serial.printf("E: lidar: send byte (2) 0x%02x\n", command);
            return false;
        }
        checksum ^= command;
        if ((command & 0x80) || (payload != nullptr && size > 0)) {
            if (lidarSerial.write(size) < 1) {
                Serial.printf("E: lidar: send byte (3) 0x%02x\n", size);
                return false;
            }
            checksum ^= size;
            for (uint8_t i = 0; i < size; i++) {
                if (lidarSerial.write(payload[i]) < 1) {
                    Serial.printf("E: lidar: send byte (%d) 0x%02x\n", i + 4, payload[i]);
                    return false;
                }
                checksum ^= payload[i];
            }
            if (lidarSerial.write(checksum) < 1) {
                Serial.printf("E: lidar: send byte (%d) 0x%02x\n", size + 4, checksum);
                return false;
            }
        }
        return true;
    }

    bool readHeader(uint8_t &type, uint8_t &subtype, uint32_t &length) {
        uint8_t c;
        if (lidarSerial.readBytes(&c, 1) < 1 || c != '\xa5') {
            Serial.println("E: lidar: read byte (1)");
            return false;
        }
        if (lidarSerial.readBytes(&c, 1) < 1 || c != '\x5a') {
            Serial.println("E: lidar: read byte (2)");
            return false;
        }
        if (lidarSerial.readBytes(&c, 1) < 1) {
            Serial.println("E: lidar: read byte (3)");
            return false;
        }
        subtype = c >> 6;
        length = c & 0x3f;
        for (uint8_t i = 1; i < 4; i ++) {
            if (lidarSerial.readBytes(&c, 1) < 1) {
                Serial.printf("E: lidar: read byte (%d)\n", i + 3);
                return false;
            }
            subtype |= c << i;
        }
        if (lidarSerial.readBytes(&type, 1) < 1) {
            Serial.println("E: lidar: read byte (7)");
            return false;
        }
        if (debugRx) {
            Serial.printf("V: lidar: rx: type=0x%02x, subtype=0x%02x, length=%d\n", type, subtype, length);
        }
        return true;
    }

    bool skipBytes(size_t length) {
        uint8_t c;
        for (size_t i = 0; i < length; i ++) {
            if (lidarSerial.readBytes(&c, 1) < 1) {
                Serial.printf("E: lidar: read byte (%d)\n", i);
                return false;
            }
            if (debugRx) {
                Serial.printf("V: lidar: skip byte (%d) 0x%02x\n", i, c);
            }
        }
        return true;
    }

    bool getDeviceInfo() {
        if (!sendCommand(0x50)) {
            return false;
        }
        uint8_t type;
        uint8_t subtype;
        uint32_t length;
        if (!readHeader(type, subtype, length)) {
            return false;
        }
        if (type != 0x04) {
            return false;
        }
        uint8_t model = 0;
        uint8_t firmwareVersion1 = 0;
        uint8_t firmwareVersion2 = 0;
        uint8_t hardwareVersion = 0;
        uint8_t serial[16] = "";
        if (length >= 1) {
            if (lidarSerial.readBytes(&model, 1) < 1) {
                return false;
            }
        }
        if (length >= 2) {
            if (lidarSerial.readBytes(&firmwareVersion1, 1) < 1) {
                return false;
            }
        }
        if (length >= 3) {
            if (lidarSerial.readBytes(&firmwareVersion2, 1) < 1) {
                return false;
            }
        }
        if (length >= 4) {
            if (lidarSerial.readBytes(&hardwareVersion, 1) < 1) {
                return false;
            }
        }
        if (length >= 20) {
            if (lidarSerial.readBytes(serial, 16) < 16) {
                return false;
            }
        }
        Serial.printf("V: lidar: device info: model=0x%02x, firmware version=%d.%d, hardware version=0x%02x\n", model, firmwareVersion1, firmwareVersion2, hardwareVersion);
        return true;
    }

    bool getDeviceHealth() {
        if (!sendCommand(0x52)) {
            return false;
        }
        uint8_t type;
        uint8_t subtype;
        uint32_t length;
        if (!readHeader(type, subtype, length)) {
            return false;
        }
        if (type != 0x06) {
            return false;
        }
        uint8_t status = 0;
        uint8_t error1 = 0;
        uint8_t error2 = 0;
        if (length >= 1) {
            if (lidarSerial.readBytes(&status, 1) < 1) {
                return false;
            }
        }
        if (length >= 2) {
            if (lidarSerial.readBytes(&error1, 1) < 1) {
                return false;
            }
        }
        if (length >= 3) {
            if (lidarSerial.readBytes(&error2, 1) < 1) {
                return false;
            }
        }
        Serial.printf("V: lidar: device health: status=0x%02x, error=0x%04x\n", status, (error1 << 0) | (error2 << 8));
        return true;
    }

    bool getLidarConf(uint32_t conf) {
        if (!sendCommand(0x84, (uint8_t *)(&conf), 4)) {
            return false;
        }
        uint8_t type;
        uint8_t subtype;
        uint32_t length;
        if (!readHeader(type, subtype, length)) {
            return false;
        }
        if (type != 0x20) {
            return false;
        }
        skipBytes(length);
        return true;
    }

    bool reset() {
        if (!sendCommand(0x40)) {
            return false;
        }
        if (!AnyLidar::reset()) {
            return false;
        }
        return true;
    }

    bool start() {
        if (!sendCommand(0x20)) {
            return false;
        }
        uint8_t type;
        uint8_t subtype;
        uint32_t length;
        if (!readHeader(type, subtype, length)) {
            return false;
        }
        if (length != 5) {
            return false;
        }
        if (!AnyLidar::start()) {
            return false;
        }
        return true;
    }

    bool stop() {
        if (!sendCommand(0x25)) {
            return false;
        }
        if (!AnyLidar::stop()) {
            return false;
        }
        return true;
    }

    virtual bool scan(uint16_t &angle, uint16_t &distance, uint8_t &strength) override {
        if (!isScan) {
            return false;
        }
        int count = 0;
        while (lidarSerial.available() < 5) {
            count ++;
            if (count > 1000) {
                Serial.println("E: lidar: read bytes (5)");
                return false;
            }
            delay(1);
        }
        uint8_t data[5];
        if (lidarSerial.readBytes(data, 5) < 5) {
            Serial.println("E: lidar: read bytes (5)");
            return false;
        }
        if ((data[0] >> 0) & 0x01 == (data[0] >> 1) & 0x01) {
            Serial.println("E: lidar: scan 2 byte");
            return false;
        }
        if (data[1] & 0x01 != 0x01) {
            Serial.println("E: lidar: check bit");
            return false;
        }
        angle = ((data[1] | (data[2] << 8)) >> 7) % 360;
        distance = (data[3] | (data[4] << 8)) >> 2;
        strength = data[0] >> 2;
        return true;
    }

};

class Lidar : public RPLidar {

};

Lidar *getLidar();

void lidarSetup();

void lidarBegin(void *params);

void lidarLoop();
