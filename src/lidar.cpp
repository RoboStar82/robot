
#include "lidar.h"

/*
void lidarSetup() {
    lidar->setup();
}

void lidarBegin(void *params) {
    while (true) {
        lidar->loop();
    }
}

void lidarStart() {
    lidar->control = lidarControlStart;
    xTaskCreatePinnedToCore(lidarBegin, "lidar", 8192, NULL, 1, NULL, 0);
}

void lidarStop() {
    lidar->control = lidarControlStop;
}

void lidarLoop() {
    lidar->loop();
}
*/

void RPLidar::begin() {
    pinMode(ROBOT_LIDAR_RX_PIN, INPUT);
    pinMode(ROBOT_LIDAR_TX_PIN, OUTPUT);
    LidarSerial.setTimeout(100);
    LidarSerial.begin(460800);
}

bool RPLidar::reset() {
    if (started) {
        return false;
    }
    if (!sendCommand(0x40)) {
        return false;
    }
    if (!AnyLidar::reset()) {
        return false;
    }
    return true;
}

bool RPLidar::start() {
    if (started) {
        return false;
    }
    skipAll();
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
    errorCount = 0;
    started = millis();
    return true;
}

bool RPLidar::stop() {
    started = 0;
    if (!sendCommand(0x25)) {
        skipAll();
        return false;
    } else {
        skipAll();
        return true;
    }
}

bool RPLidar::sendCommand(uint8_t command, uint8_t *payload, uint8_t size) {
#if LIDAR_DEBUG_TX
    if (payload != nullptr && size > 0) {
        log_i("Lidar: tx: command=0x%02x, payload size=%d", command, size);
    } else {
        log_i("Lidar: tx: command=0x%02x", command);
    }
#endif
    uint8_t checksum = 0;
    if (LidarSerial.write(0xa5) < 1) {
        log_e("Lidar: send byte (1) 0xa5");
        return false;
    }
    checksum ^= 0xa5;
    if (LidarSerial.write(command) < 1) {
        log_e("Lidar: send byte (2) 0x%02x", command);
        return false;
    }
    checksum ^= command;
    if ((command & 0x80) || (payload != nullptr && size > 0)) {
        if (LidarSerial.write(size) < 1) {
            log_e("Lidar: send byte (3) 0x%02x", size);
            return false;
        }
        checksum ^= size;
        for (int i = 0; i < size; i++) {
            if (LidarSerial.write(payload[i]) < 1) {
                log_e("Lidar: send byte (%d) 0x%02x", i + 4, payload[i]);
                return false;
            }
            checksum ^= payload[i];
        }
        if (LidarSerial.write(checksum) < 1) {
            log_e("Lidar: send byte (%d) 0x%02x", size + 4, checksum);
            return false;
        }
    }
    return true;
}

bool RPLidar::readHeader(uint8_t &type, uint8_t &subtype, uint32_t &length) {
    uint8_t c;
    if (LidarSerial.readBytes(&c, 1) < 1 || c != '\xa5') {
        log_e("Lidar: read byte (1)");
        return false;
    }
    if (LidarSerial.readBytes(&c, 1) < 1 || c != '\x5a') {
        log_e("Lidar: read byte (2)");
        return false;
    }
    if (LidarSerial.readBytes(&c, 1) < 1) {
        log_e("Lidar: read byte (3)");
        return false;
    }
    subtype = c >> 6;
    length = c & 0x3f;
    for (int i = 1; i < 4; i++) {
        if (LidarSerial.readBytes(&c, 1) < 1) {
            log_e("Lidar: read byte (%d)", i + 3);
            return false;
        }
        subtype |= c << i;
    }
    if (LidarSerial.readBytes(&type, 1) < 1) {
        log_e("Lidar: read byte (7)");
        return false;
    }
#if LIDAR_DEBUG_RX
    log_i("Lidar: rx: type=0x%02x, subtype=0x%02x, length=%d", type, subtype, length);
#endif
    return true;
}

bool RPLidar::skipAll() {
    uint8_t c;
    while (LidarSerial.available()) {
        LidarSerial.readBytes(&c, 1);
    }
    return true;
}

bool RPLidar::skipBytes(size_t length) {
    uint8_t c;
    for (size_t i = 0; i < length; i++) {
        if (LidarSerial.readBytes(&c, 1) < 1) {
            log_e("Lidar: read byte (%d)", i);
            return false;
        }
#if LIDAR_DEBUG_RX
        log_i("Lidar: skip byte (%d) 0x%02x", i, c);
#endif
    }
    return true;
}

bool RPLidar::getDeviceInfo() {
    if (started) {
        return false;
    }
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
        if (LidarSerial.readBytes(&model, 1) < 1) {
            return false;
        }
    }
    if (length >= 2) {
        if (LidarSerial.readBytes(&firmwareVersion1, 1) < 1) {
            return false;
        }
    }
    if (length >= 3) {
        if (LidarSerial.readBytes(&firmwareVersion2, 1) < 1) {
            return false;
        }
    }
    if (length >= 4) {
        if (LidarSerial.readBytes(&hardwareVersion, 1) < 1) {
            return false;
        }
    }
    if (length >= 20) {
        if (LidarSerial.readBytes(serial, 16) < 16) {
            return false;
        }
    }
    log_i("Lidar: device info: model=0x%02x, firmware version=%d.%d, hardware version=0x%02x", model, firmwareVersion1, firmwareVersion2, hardwareVersion);
    return true;
}

bool RPLidar::getDeviceHealth() {
    if (started) {
        return false;
    }
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
        if (LidarSerial.readBytes(&status, 1) < 1) {
            return false;
        }
    }
    if (length >= 2) {
        if (LidarSerial.readBytes(&error1, 1) < 1) {
            return false;
        }
    }
    if (length >= 3) {
        if (LidarSerial.readBytes(&error2, 1) < 1) {
            return false;
        }
    }
    log_i("Lidar: device health: status=0x%02x, error=0x%04x", status, (error1 << 0) | (error2 << 8));
    return true;
}

bool RPLidar::getLidarConf(uint32_t conf) {
    if (started) {
        return false;
    }
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

bool RPLidar::scan(uint16_t &angle, uint16_t &distance, uint8_t &strength) {
    if (!started) {
        return false;
    }
    int count = 0;
    while (LidarSerial.available() < 5) {
        count++;
        if (count > 1000) {
            log_e("Lidar: read bytes (5)");
            return false;
        }
        delay(1);
    }
    uint8_t data[5];
    if (LidarSerial.readBytes(data, 5) < 5) {
        log_e("Lidar: read bytes (5)");
        return false;
    }
    if ((data[0] >> 0) & 0x01 == (data[0] >> 1) & 0x01) {
        log_e("Lidar: scan 2 byte");
        return false;
    }
    if (data[1] & 0x01 != 0x01) {
        log_e("Lidar: check bit");
        return false;
    }
    angle = ((data[1] | (data[2] << 8)) >> 7) % 360;
    distance = (data[3] | (data[4] << 8)) >> 2;
    strength = data[0] >> 2;
    return true;
}

Lidar lidar = Lidar();
