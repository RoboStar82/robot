
#pragma once

#include <Arduino.h>

#ifndef LidarSerial
#define LidarSerial Serial1
#endif

#ifndef ROBOT_LIDAR_RX_PIN
#define ROBOT_LIDAR_RX_PIN 15
#endif
#ifndef ROBOT_LIDAR_TX_PIN
#define ROBOT_LIDAR_TX_PIN 16
#endif
#ifndef LIDAR_DEBUG_TX
#define LIDAR_DEBUG_TX 0
#endif
#ifndef LIDAR_DEBUG_RX
#define LIDAR_DEBUG_RX 0
#endif

typedef struct {
    int angle = 0;
    int angle0 = 0;
    int angle1 = 0;
    int distance = 0;
    int distance0 = 0;
    int distance1 = 0;
    int width = 0;
} road_object_t;

class Lidar {
   public:
    Lidar();
    ~Lidar();

    void begin();

    bool start();
    bool stop();

    void scanRoadObjects(road_object_t* objects, int& objectCount, int objectCountMax);
    void addRoadObject(road_object_t object, road_object_t* objects, int& objectCount, int objectCountMax);

    void task();

    static void task(void* arg);

    bool getDeviceInfo();

    bool getDeviceHealth();

    bool getLidarConf(uint32_t conf);

   protected:
    // Начато ли сканирование
    TaskHandle_t startedTask = nullptr;
    // Закончено ли сканирование
    bool needStop = true;

    // Расстояния
    uint16_t distances[360];

    // Синусы углов от 0 до 90: 0..4096
    uint16_t sinuses[91] = {0, 71, 143, 214, 286, 357, 428, 499, 570, 641, 711, 782, 852, 921, 991, 1060, 1129, 1198, 1266, 1334, 1401, 1468, 1534, 1600, 1666, 1731, 1796, 1860, 1923, 1986, 2048, 2110, 2171, 2231, 2290, 2349, 2408, 2465, 2522, 2578, 2633, 2687, 2741, 2793, 2845, 2896, 2946, 2996, 3044, 3091, 3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435, 3474, 3511, 3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798, 3824, 3849, 3873, 3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021, 4034, 4046, 4056, 4065, 4074, 4080, 4086, 4090, 4094, 4095, 4096};

    // Косинусы углов от 0 до 90: 0..4096
    uint16_t cosines[91] = {4096, 4095, 4094, 4090, 4086, 4080, 4074, 4065, 4056, 4046, 4034, 4021, 4006, 3991, 3974, 3956, 3937, 3917, 3896, 3873, 3849, 3824, 3798, 3770, 3742, 3712, 3681, 3650, 3617, 3582, 3547, 3511, 3474, 3435, 3396, 3355, 3314, 3271, 3228, 3183, 3138, 3091, 3044, 2996, 2946, 2896, 2845, 2793, 2741, 2687, 2633, 2578, 2522, 2465, 2408, 2349, 2290, 2231, 2171, 2110, 2048, 1986, 1923, 1860, 1796, 1731, 1666, 1600, 1534, 1468, 1401, 1334, 1266, 1198, 1129, 1060, 991, 921, 852, 782, 711, 641, 570, 499, 428, 357, 286, 214, 143, 71, 0};

    bool scan(uint16_t& angle, uint16_t& distance, uint8_t& strength);

    bool sendStartCommand();
    bool sendResetCommand();
    bool sendStopCommand();

    bool sendCommand(uint8_t command, uint8_t* payload = nullptr, uint8_t size = 0);

    bool readHeader(uint8_t& type, uint8_t& subtype, uint32_t& length);

    bool skipBytes(size_t length);

    bool skipAll();

    void zero();

   protected:
};

extern Lidar lidar;
