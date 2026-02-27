# 1 "ble_battery.h"

#pragma once

#include <NimBLEDevice.h>

// server write client read
class BLEBatteryLevel {
   public:
    BLEBatteryLevel();
    ~BLEBatteryLevel();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void setValue(uint8_t value);

    void end();

    void task();

    static void task(void* arg);

   protected:
    BLEUUID characteristicUuid = BLEUUID((uint16_t)0x2a19);
    const char* characteristicDescription = "Battery Level";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;

    bool taskCreated = false;
    uint8_t value = 0;
#if ROBOT_HAS_TRANSCEIVER_LORA
    uint8_t batteryPin = 2;
    uint32_t minVoltage = 1500;
    uint32_t maxVoltage = 2000;
#else
    uint8_t batteryPin = 0;
    uint32_t minVoltage = 0;
    uint32_t maxVoltage = 0;
#endif
};

class BLEBattery {
   public:
    BLEService* service = nullptr;

    // server write client read
    BLEBatteryLevel level;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID((uint16_t)0x180f);
};

# 1 "ble_robot.h"

#pragma once

#include <NimBLEDevice.h>

// server write client read
// client write server read
class BLERobotSettings : BLECharacteristicCallbacks {
   public:
    BLERobotSettings();
    ~BLERobotSettings();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0004-4000-8000-000000000000");
    const char* characteristicDescription = "Settings";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
// client write server read
class BLERobotOtaMode : BLECharacteristicCallbacks {
   public:
    BLERobotOtaMode();
    ~BLERobotOtaMode();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0009-4000-8000-000000000000");
    const char* characteristicDescription = "OTA Mode";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;
};

// server write client read
// client write server read
class BLERobotWiFiMode : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiMode();
    ~BLERobotWiFiMode();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0010-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi Mode";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;
};

// server write client read
// client write server read
class BLERobotWiFiSSID : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiSSID();
    ~BLERobotWiFiSSID();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0011-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi SSID";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
// client write server read
class BLERobotWiFiPassword : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiPassword();
    ~BLERobotWiFiPassword();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0012-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi Password";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

class BLERobot {
   public:
    BLERobot();
    ~BLERobot();

    BLEService* service = nullptr;

    // server write client read
    // client write server read
    BLERobotSettings settings;
    // server write client read
    // client write server read
    BLERobotOtaMode otaMode;
    // server write client read
    // client write server read
    BLERobotWiFiMode wifiMode;
    // server write client read
    // client write server read
    BLERobotWiFiSSID wifiSSID;
    // server write client read
    // client write server read
    BLERobotWiFiPassword wifiPassword;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID("b0b0c7ab-0000-4000-8000-000000000000");
};

# 1 "ble_uart.h"

#pragma once

#include <NimBLEDevice.h>

// client write server read
class BLEUartRx : BLECharacteristicCallbacks {
   public:
    BLEUartRx();
    ~BLEUartRx();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    const char* characteristicDescription = "UART Rx";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
class BLEUartTx {
   public:
    BLEUartTx();
    ~BLEUartTx();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
    const char* characteristicDescription = "UART Tx";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

class BLEUart : public BLECharacteristicCallbacks {
   public:
    BLEUart();
    ~BLEUart();

    BLEService* service = nullptr;

    // client write server read
    BLEUartRx rx;

    // server write client read
    BLEUartTx tx;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
};

# 1 "ble.h"

#pragma once

#include <NimBLEDevice.h>

#include "ble_battery.h"
#include "ble_robot.h"
#include "ble_uart.h"

class BLE : BLEServerCallbacks {
   public:
    BLE();
    ~BLE();

    BLEServer* server = nullptr;
    BLEAdvertising* advertising = nullptr;

    BLEBattery battery;
    BLERobot robot;
    BLEUart uart;

    void begin();
    void startAdvertising();
    void stopAdvertising();
    void end();

    void onConnect(BLEServer* bleServer, BLEConnInfo& connInfo);
    void onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason);

   protected:
    bool started = false;
    bool connected = false;
    uint32_t advertisingDuration = 9999;
};

extern BLE ble;

# 1 "controller.h"

#pragma once

#include <Arduino.h>

#if ROBOT_HAS_CONTROLLER_SERIAL
#ifndef ROBOT_CONTROLLER_SERIAL_RX_PIN
#define ROBOT_CONTROLLER_SERIAL_RX_PIN 19
#endif
#ifndef ROBOT_CONTROLLER_SERIAL_TX_PIN
#define ROBOT_CONTROLLER_SERIAL_TX_PIN 20
#endif
#ifndef ControllerSerial
#define ControllerSerial Serial2
#endif
#endif

#if ROBOT_HAS_TRANSCEIVER_SERIAL
#ifndef ROBOT_TRANSCEIVER_SERIAL_RX_PIN
#define ROBOT_TRANSCEIVER_SERIAL_RX_PIN 19
#endif
#ifndef ROBOT_TRANSCEIVER_SERIAL_TX_PIN
#define ROBOT_TRANSCEIVER_SERIAL_TX_PIN 20
#endif
#ifndef ControllerSerial
#define ControllerSerial Serial2
#endif
#endif

typedef struct {
    int8_t lx : 4 = 0;
    int8_t ly : 4 = 0;
    int8_t rx : 4 = 0;
    int8_t ry : 4 = 0;
    int8_t dx : 2 = 0;
    int8_t dy : 2 = 0;
    int8_t lz : 2 = 0;
    int8_t rz : 2 = 0;
    bool lt : 1 = 0;
    bool rt : 1 = 0;
    bool a : 1 = 0;
    bool b : 1 = 0;
    bool x : 1 = 0;
    bool y : 1 = 0;
    bool start : 1 = 0;
    bool back : 1 = 0;
    uint8_t mode : 8 = 0;
} __attribute__((packed)) controller_state_t;

class Controller {
   public:
    Controller();
    ~Controller();

    void begin();

    controller_state_t getState();

    void getState(uint8_t state[]);

    void getState(controller_state_t* state);

    void setState(controller_state_t newState);

    void setState(uint8_t newState[]);

    void setState(controller_state_t* newState);

    void onChange(controller_state_t oldState);

    void print();

    void needSendState();

    void task();

    static void task(void* arg);

   protected:
    controller_state_t state;

    QueueHandle_t needQueue = xQueueCreate(4, sizeof(bool));

    void sendState();
};

extern Controller controller;

# 1 "encoder.h"

#pragma once

#include <Arduino.h>

#include "rotaryDecoder.h"

class Encoder {
   public:
    Encoder(int8_t i2cAddress, int8_t interruptPin);
    ~Encoder();
    void begin();

    void task();
    static void task(void* arg);
    static void interrupt();

    bool needUpdate = false;

   protected:
    int8_t i2cAddress = 0;
    int8_t interruptPin = 0;
    rotaryDecoder* decoder = nullptr;
};

#ifndef ROBOT_MOTOR_ENCODER_I2C_ADDRESS
#define ROBOT_MOTOR_ENCODER_I2C_ADDRESS 0x20
#endif

#ifndef ROBOT_MOTOR_ENCODER_INTERRUPT_PIN
#define ROBOT_MOTOR_ENCODER_INTERRUPT_PIN 0
#endif

extern Encoder encoder;

# 1 "imu.h"

#pragma once

#include <Arduino.h>
#include <iarduino_Position_BMX055.h>

class IMU {
   public:
    IMU();
    ~IMU();

    void begin();

    int getAxisX();
    int getAxisY();
    int getAxisZ();

    void calibrate(int time = 10000);

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
    iarduino_Position_BMX055* bmx = nullptr;
};

extern IMU imu;

# 1 "led.h"

#pragma once

#include <Arduino.h>

typedef struct {
    struct {
        bool on = false;
    } power;
    struct {
        char button = 0;
    } controller;
    struct {
        bool errors = false;
        bool sending = false;
        bool sleeping = false;
    } lora;
    struct {
        bool ble = false;
        bool wifi = false;
    } ota;
} led_state_t;

typedef struct {
    struct {
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } on;
    } power;
    struct {
        struct {
            bool value = false;
            unsigned int period = 19;
            unsigned int counter = 0;
        } errors;
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } sleeping;
    } lora;
    struct {
        struct {
            bool value = false;
            unsigned int period = 399;
            unsigned int counter = 0;
        } ble;
        struct {
            bool value = false;
            unsigned int period = 399;
            unsigned int counter = 0;
        } wifi;
    } ota;
} led_timers_t;

class Led {
   public:
    Led();
    ~Led();

    void begin();

    void onChange();

    void setPowerOn(bool value);

    void setControllerButton(char value);

    void setLoraErrors(bool value);
    void setLoraSending(bool value);
    void setLoraSleeping(bool value);

    void setOtaBLE(bool value);
    void setOtaWiFi(bool value);

    void task();

   protected:
    uint8_t rgbPin = 0;
    uint8_t ledPin = 0;
    led_timers_t timers;
    led_state_t state;
    bool changed = false;
    int counter = 0;

    static void task(void* arg);
};

extern Led led;

# 1 "lidar.h"

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

# 1 "lora.h"

#pragma once

// https://github.com/jgromes/RadioLib
// https://github.com/ropg/heltec_esp32_lora_v3

#include <RadioLib.h>

#if ROBOT_HAS_CONTROLLER_LORA
#define DIO1 14
#define RST_LoRa 3
#define BUSY_LoRa 46
#endif

class Lora {
   public:
    Lora();
    ~Lora();

    void begin();

    void onPacketSent();
    void onPacketReceived();

    void needSendControllerState();

    void reset();
    void task();

   protected:
    TaskHandle_t startedTask = nullptr;

    SX1262* module = nullptr;

    int errors = 0;
    int delays = 0;
    bool sending = false;
    bool sleeping = false;

    bool needRead = false;
    bool needSend = false;
    bool doneSend = false;

    bool readControllerState();
    bool sendControllerState();

    static void packetSentCallback();
    static void packetReceivedCallback();

    static void task(void* arg);
};

extern Lora lora;

# 1 "motor.h"

#pragma once

#include <Arduino.h>
#include <driver/mcpwm_prelude.h>

class Motor {
   public:
    Motor(const char* name);
    ~Motor();

    void begin();

    const char* getName();

    void setMinSpeed(uint value);
    void setMaxSpeed(uint value);

    int getEncoderSpeed();
    void setEncoderSpeed(int value);

    void setSpeed(int value);

   protected:
    const char* name = "";
    int encoderSpeed = 0;
    bool encoderWorks = false;
    int increaseSpeed = 0;
    uint minSpeed = 0;
    uint maxSpeed = 255;
    int speed = 0;
};

class MotorPWM : Motor {
   public:
    MotorPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);
    ~MotorPWM();

    void begin();

    using Motor::getName;

    using Motor::getEncoderSpeed;
    using Motor::setEncoderSpeed;

    using Motor::setMaxSpeed;
    using Motor::setMinSpeed;

    void setSpeed(int value);

   protected:
    using Motor::maxSpeed;
    using Motor::minSpeed;
    using Motor::encoderSpeed;
    using Motor::encoderWorks;
    using Motor::increaseSpeed;
    using Motor::name;
    using Motor::speed;
    uint8_t pwmPin1 = 0;
    uint8_t pwmPin2 = 0;
};

class MotorMCPWM : MotorPWM {
   public:
    MotorMCPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);
    ~MotorMCPWM();

    void begin();

    using MotorPWM::getName;

    using MotorPWM::getEncoderSpeed;
    using MotorPWM::setEncoderSpeed;

    using MotorPWM::setMaxSpeed;
    using MotorPWM::setMinSpeed;

    void setSpeed(int value);

   protected:
    using MotorPWM::maxSpeed;
    using MotorPWM::minSpeed;
    using MotorPWM::encoderSpeed;
    using MotorPWM::encoderWorks;
    using MotorPWM::increaseSpeed;
    using MotorPWM::name;
    using MotorPWM::pwmPin1;
    using MotorPWM::pwmPin2;
    using MotorPWM::speed;

    bool isLeft = false;
    bool isRight = false;
    bool isCenter = false;
    bool isFront = false;
    bool isBack = false;

    mcpwm_timer_handle_t mcpwmTimer = nullptr;
    mcpwm_oper_handle_t mcpwmOperator = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator1 = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator2 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator1 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator2 = nullptr;
};

#ifndef ROBOT_MOTOR_LF_PIN1
#define ROBOT_MOTOR_LF_PIN1 1
#endif
#ifndef ROBOT_MOTOR_LF_PIN2
#define ROBOT_MOTOR_LF_PIN2 2
#endif
#ifndef ROBOT_MOTOR_RF_PIN1
#define ROBOT_MOTOR_RF_PIN1 5
#endif
#ifndef ROBOT_MOTOR_RF_PIN2
#define ROBOT_MOTOR_RF_PIN2 4
#endif
#ifndef ROBOT_MOTOR_LB_PIN1
#define ROBOT_MOTOR_LB_PIN1 42
#endif
#ifndef ROBOT_MOTOR_LB_PIN2
#define ROBOT_MOTOR_LB_PIN2 41
#endif
#ifndef ROBOT_MOTOR_RB_PIN1
#define ROBOT_MOTOR_RB_PIN1 6
#endif
#ifndef ROBOT_MOTOR_RB_PIN2
#define ROBOT_MOTOR_RB_PIN2 7
#endif
#ifndef ROBOT_MOTOR_CC_PIN1
#define ROBOT_MOTOR_CC_PIN1 45
#endif
#ifndef ROBOT_MOTOR_CC_PIN2
#define ROBOT_MOTOR_CC_PIN2 47
#endif

#if ROBOT_HAS_MOTOR_MCPWM
extern MotorMCPWM motorLF;
extern MotorMCPWM motorRF;
extern MotorMCPWM motorLB;
extern MotorMCPWM motorRB;
#elif ROBOT_HAS_MOTOR_PWM
extern MotorPWM motorLF;
extern MotorPWM motorRF;
extern MotorPWM motorLB;
extern MotorPWM motorRB;
#else
extern Motor motorLF;
extern Motor motorRF;
extern Motor motorLB;
extern Motor motorRB;
#endif

#if ROBOT_HAS_MOTOR_1_MCPWM
extern MotorMCPWM motor1;
#elif ROBOT_HAS_MOTOR_1_PWM
extern MotorPWM motor1;
#else
extern Motor motor1;
#endif
#if ROBOT_HAS_MOTOR_2_MCPWM
extern MotorMCPWM motor2;
#elif ROBOT_HAS_MOTOR_2_PWM
extern MotorPWM motor2;
#else
extern Motor motor2;
#endif
#if ROBOT_HAS_MOTOR_3_MCPWM
extern MotorMCPWM motor3;
#elif ROBOT_HAS_MOTOR_3_PWM
extern MotorPWM motor3;
#else
extern Motor motor3;
#endif

# 1 "navigation.h"

#pragma once

#include <Arduino.h>

#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
#define NavigationSerial Serial2
#endif

class Navigation {
   public:
    Navigation();
    ~Navigation();

    void begin();

    void task();

    static void task(void* arg);
};

extern Navigation navigation;

# 1 "ota.h"

#pragma once

#include <ArduinoOTA.h>
#include <WiFi.h>

typedef enum {
    OTA_OFF = 0,
    OTA_BLE = 1,
    OTA_WIFI = 2,
    OTA_ALL = 7,
} ota_mode_t;

class OTA {
   public:
    OTA();
    ~OTA();

    void begin();

    void beginBLE();
    void beginWiFi();

    void enableBLE();
    void enableWiFi();

    void disableBLE();
    void disableWiFi();

    void needEnableBLE();
    void needEnableWiFi();

    void needDisableBLE();
    void needDisableWiFi();

    void endBLE();
    void endWiFi();

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(int8_t));

    ota_mode_t otaMode = OTA_OFF;

    wifi_mode_t wifiMode = WIFI_MODE_NULL;
    wl_status_t wifiStatus = WL_NO_SHIELD;
    bool wifiConnected = false;
};

extern OTA ota;

# 1 "robot.h"

#pragma once

#include <Arduino.h>

typedef enum {
    ROBOT_UPDATE_STOP = 0,
    ROBOT_UPDATE_START = 1,
    ROBOT_UPDATE_SPEED = 2,
    ROBOT_UPDATE_SERVO = 3,
} robot_update_t;

class Robot {
   public:
    Robot();
    ~Robot();

    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void autoStart();
    void autoStop();
    void autoEnd();
    void updateSpeed();
    void updateServo();
    void updateCount();

    void needUpdateAutoStart();
    void needUpdateAutoStop();
    void needUpdateSpeed();
    void needUpdateServo();

    void stop();

    void task();

    static void task(void* arg);

    void autoTask();

    static void autoTask(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;
    TaskHandle_t autoStartedTask = nullptr;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(robot_update_t));
    int countX = 0;
    int countY = 0;
    int countLZ = 0;
    int countRZ = 0;
    int countDX = 0;
    bool wheelDown = false;
};

extern Robot robot;

# 1 "servo.h"

#pragma once

#include <Arduino.h>

class Servo {
   public:
    Servo(const char* name);
    ~Servo();

    void begin(float angle = 90);

    const char* getName();

    void setMinAngle(float value);
    void setMaxAngle(float value);

    void setAngle(float value, bool force = false);

   protected:
    const char* name;
    float minAngle = 0.0f;
    float maxAngle = 180.0f;
    float angle = 90.0f;
};

class ServoPWM : Servo {
   public:
    ServoPWM(const char* name, uint8_t pin);
    ~ServoPWM();

    void begin(float angle = 90);

    using Servo::getName;

    using Servo::setMaxAngle;
    using Servo::setMinAngle;

    void setAngle(float value, bool force = false);

   protected:
    using Servo::angle;
    using Servo::maxAngle;
    using Servo::minAngle;
    using Servo::name;
    uint8_t pwmPin = 0;
};

#if ROBOT_HAS_SERVO_1
extern ServoPWM servo1;
#else
extern Servo servo1;
#endif
#if ROBOT_HAS_SERVO_2
extern ServoPWM servo2;
#else
extern Servo servo2;
#endif
#if ROBOT_HAS_SERVO_3
extern ServoPWM servo3;
#else
extern Servo servo3;
#endif
#if ROBOT_HAS_SERVO_4
extern ServoPWM servo4;
#else
extern Servo servo4;
#endif
#if ROBOT_HAS_SERVO_5
extern ServoPWM servo5;
#else
extern Servo servo5;
#endif
#if ROBOT_HAS_SERVO_6
extern ServoPWM servo6;
#else
extern Servo servo6;
#endif
#if ROBOT_HAS_SERVO_7
extern ServoPWM servo7;
#else
extern Servo servo7;
#endif
#if ROBOT_HAS_SERVO_8
extern ServoPWM servo8;
#else
extern Servo servo8;
#endif

# 1 "settings.h"

#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "ota.h"

class Settings : Preferences {
   public:
    Settings();
    ~Settings();

    void begin();

    void read();

    void end();

    ota_mode_t getOtaMode();
    bool hasOtaBLE();
    bool hasOtaWiFi();
    bool setOtaMode(ota_mode_t value);
    bool addOtaBLE();
    bool addOtaWiFi();
    bool removeOtaBLE();
    bool removeOtaWiFi();

    wifi_mode_t getWiFiMode();
    String getWiFiSSID();
    String getWiFiPassword();
    bool setWiFiMode(wifi_mode_t value);
    bool setWiFiSSID(String value);
    bool setWiFiPassword(String value);

    String getRobotSettings();
    bool setRobotSettings(String value);

   protected:
    bool started = false;

    ota_mode_t otaMode = OTA_OFF;

    wifi_mode_t wifiMode = WIFI_MODE_NULL;
    String wifiSSID = "";
    String wifiPassword = "";

    String robotSettings = "";

    using Preferences::begin;

    using Preferences::isKey;

    using Preferences::getBool;
    using Preferences::getBytes;
    using Preferences::getBytesLength;
    using Preferences::getChar;
    using Preferences::getDouble;
    using Preferences::getFloat;
    using Preferences::getInt;
    using Preferences::getLong;
    using Preferences::getLong64;
    using Preferences::getShort;
    using Preferences::getString;
    using Preferences::getType;
    using Preferences::getUChar;
    using Preferences::getUInt;
    using Preferences::getULong;
    using Preferences::getULong64;
    using Preferences::getUShort;

    using Preferences::putBool;
    using Preferences::putBytes;
    using Preferences::putChar;
    using Preferences::putDouble;
    using Preferences::putFloat;
    using Preferences::putInt;
    using Preferences::putLong;
    using Preferences::putLong64;
    using Preferences::putShort;
    using Preferences::putString;
    using Preferences::putUChar;
    using Preferences::putUInt;
    using Preferences::putULong;
    using Preferences::putULong64;
    using Preferences::putUShort;
};

extern Settings settings;

# 1 "tea.h"

#pragma once

#include <Arduino.h>

class TEA {
   public:
    TEA();
    ~TEA();

    bool encryptData(uint8_t* data);
    bool decryptData(uint8_t* data);
    void encryptBlock(uint32_t* block);
    void decryptBlock(uint32_t* block);

   protected:
    uint32_t count = 4;
    uint32_t delta = 0x00;
    uint32_t sumDelta = 4 * 0x00;
    uint32_t key[4] = {0x00, 0x00, 0x00, 0x00};
};

extern TEA tea;

# 1 "usb.h"

#pragma once

#include <Arduino.h>
#include <usb/usb_host.h>

#include "hid_host.h"

typedef struct {
    hid_host_device_handle_t device_handle;
    hid_host_driver_event_t driver_event;
    void* arg;
} usb_hid_host_device_event_t;

typedef struct {
    uint8_t lx;
    uint8_t ly;
    uint8_t rx;
    uint8_t ry;
    uint8_t dpad : 4;
    uint8_t x : 1;
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t y : 1;
    uint8_t lzu : 1;
    uint8_t rzu : 1;
    uint8_t lzd : 1;
    uint8_t rzd : 1;
    uint8_t back : 1;
    uint8_t start : 1;
    uint8_t lt : 1;
    uint8_t rt : 1;
    uint8_t mode : 8;
    uint8_t b8;
} __attribute__((packed)) usb_hid_controller_0216_state_t;

typedef struct {
    uint8_t b1;
    uint8_t lx;
    uint8_t ly;
    uint8_t rx;
    uint8_t ry;
    uint8_t dpad : 4;
    uint8_t x : 1;
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t y : 1;
    uint8_t lzu : 1;
    uint8_t rzu : 1;
    uint8_t lzd : 1;
    uint8_t rzd : 1;
    uint8_t back : 1;
    uint8_t start : 1;
    uint8_t lt : 1;
    uint8_t rt : 1;
    uint8_t mode : 8;
} __attribute__((packed)) usb_hid_controller_0219_state_t;

class USB {
   public:
    USB();
    ~USB();

    void begin();

   protected:
    static void hostLibTask(void* arg);

    static QueueHandle_t hidHostQueue;
    static void hidHostTask(void* arg);

    static void hidHostDriverEventHandler(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg);
    static void hidHostDriverEventCallback(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg);
    static void hidHostInterfaceEventCallback(hid_host_device_handle_t device_handle, const hid_host_interface_event_t interface_event, void* arg);
    static void hidHostInterfaceInputReportEventHandler(hid_host_device_handle_t device_handle);
};

extern USB usb;

# 1 "version.h"

#define BUILD_DATETIME "2026-02-27 13:09:48"
#define BUILD_TIMESTAMP 1772186988

# 1 "ble_battery.cpp"

#include "ble.h"

BLEBatteryLevel::BLEBatteryLevel() {}

BLEBatteryLevel::~BLEBatteryLevel() {}

void BLEBatteryLevel::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    if (batteryPin) {
        pinMode(batteryPin, INPUT);
        if (!taskCreated) {
            xTaskCreate(task, "battery_level_task", 4096, NULL, 1, NULL);
            taskCreated = true;
        }
    }
}

void BLEBatteryLevel::end() {
    characteristic = nullptr;
}

void BLEBatteryLevel::task(void* arg) {
    ble.battery.level.task();
}

void BLEBatteryLevel::task() {
    uint32_t voltage = 0;
    int lowCount = 0;
    while (true) {
        uint32_t voltages[4] = {0, 0, 0, 0};
        voltage = analogReadMilliVolts(batteryPin);
        voltages[0] = voltage;
        vTaskDelay(9);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else {
            voltages[1] = voltage;
        }
        vTaskDelay(9);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[2] = voltages[1];
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else if (voltage < voltages[1]) {
            voltages[2] = voltages[1];
            voltages[1] = voltage;
        } else {
            voltages[2] = voltage;
        }
        vTaskDelay(9);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[3] = voltages[2];
            voltages[2] = voltages[1];
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else if (voltage < voltages[1]) {
            voltages[3] = voltages[2];
            voltages[2] = voltages[1];
            voltages[1] = voltage;
        } else if (voltage < voltages[2]) {
            voltages[3] = voltages[2];
            voltages[2] = voltage;
        } else {
            voltages[3] = voltage;
        }
        voltage = (voltages[1] + voltages[2]) >> 1;
        if (voltage > maxVoltage) {
            setValue(100);
            lowCount = 0;
        } else if (voltage > minVoltage) {
            setValue(round(100.0f * (voltage - minVoltage) / (maxVoltage - minVoltage)));
            lowCount = 0;
        } else if (voltage > 0) {
            if (++lowCount > 9) {
                esp_deep_sleep_start();
            }
        }
        vTaskDelay(9999);
    }
}

void BLEBatteryLevel::setValue(uint8_t _value) {
    if (value == _value) {
        return;
    }
    value = _value;
    log_i("Battery: %d%%", value);
    if (characteristic) {
        characteristic->setValue(value);
        characteristic->indicate();
    }
}

void BLEBattery::begin() {
    service = ble.server->createService(serviceUuid);
    level.begin(service);
    service->start();
}

void BLEBattery::end() {
    level.end();
    service = nullptr;
}

# 1 "ble_robot.cpp"

#include "ble.h"
#include "ota.h"
#include "settings.h"

BLERobotSettings::BLERobotSettings() {}

BLERobotSettings::~BLERobotSettings() {}

void BLERobotSettings::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getRobotSettings());
    characteristic->setCallbacks(this);
}

void BLERobotSettings::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setRobotSettings(value.c_str());
}

void BLERobotSettings::end() {
    characteristic = nullptr;
}

BLERobotOtaMode::BLERobotOtaMode() {}

BLERobotOtaMode::~BLERobotOtaMode() {}

void BLERobotOtaMode::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getOtaMode());
    characteristic->setCallbacks(this);
}

void BLERobotOtaMode::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int8_t otaMode = value.length() > 0 ? value[0] : 0;
    settings.setOtaMode((ota_mode_t)otaMode);
    ota.begin();
}

void BLERobotOtaMode::end() {
    characteristic = nullptr;
}

BLERobotWiFiMode::BLERobotWiFiMode() {}

BLERobotWiFiMode::~BLERobotWiFiMode() {}

void BLERobotWiFiMode::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getWiFiMode());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiMode::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int8_t wifiMode = value.length() > 0 ? value[0] : 0;
    settings.setWiFiMode((wifi_mode_t)wifiMode);
    ota.begin();
}

void BLERobotWiFiMode::end() {
    characteristic = nullptr;
}

BLERobotWiFiSSID::BLERobotWiFiSSID() {}

BLERobotWiFiSSID::~BLERobotWiFiSSID() {}

void BLERobotWiFiSSID::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getWiFiSSID());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiSSID::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setWiFiSSID(value.c_str());
}

void BLERobotWiFiSSID::end() {
    characteristic = nullptr;
}

BLERobotWiFiPassword::BLERobotWiFiPassword() {}

BLERobotWiFiPassword::~BLERobotWiFiPassword() {}

void BLERobotWiFiPassword::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getWiFiPassword());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiPassword::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setWiFiPassword(value.c_str());
}

void BLERobotWiFiPassword::end() {
    characteristic = nullptr;
}

BLERobot::BLERobot() {}

BLERobot::~BLERobot() {}

void BLERobot::begin() {
    service = ble.server->createService(serviceUuid);
    settings.begin(service);
    otaMode.begin(service);
    wifiMode.begin(service);
    wifiSSID.begin(service);
    wifiPassword.begin(service);
    service->start();
}

void BLERobot::end() {
    settings.end();
    otaMode.end();
    wifiMode.end();
    wifiSSID.end();
    wifiPassword.end();
    service = nullptr;
}
# 1 "ble_uart.cpp"

#include "ble.h"

BLEUartRx::BLEUartRx() {}

BLEUartRx::~BLEUartRx() {}

void BLEUartRx::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // client write server read
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setCallbacks(this);
}

void BLEUartRx::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    log_i("rx: %s", value.c_str());
}

void BLEUartRx::end() {
    characteristic = nullptr;
}

BLEUartTx::BLEUartTx() {}

BLEUartTx::~BLEUartTx() {}

void BLEUartTx::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
}

void BLEUartTx::end() {
    characteristic = nullptr;
}

BLEUart::BLEUart() {}

BLEUart::~BLEUart() {}

void BLEUart::begin() {
    service = ble.server->createService(serviceUuid);
    rx.begin(service);
    tx.begin(service);
    service->start();
}

void BLEUart::end() {
    rx.end();
    tx.end();
    service = nullptr;
}

# 1 "ble.cpp"

#include "ble.h"

BLE ble;

BLE::BLE() {}

BLE::~BLE() {}

void BLE::begin() {
    if (!started) {
        started = true;
        log_i("BLE: Init");
        BLEDevice::init("");
#if BLE_SECURITY_PASSKEY
        BLEDevice::setSecurityAuth(true, true, true);
        BLEDevice::setSecurityPasskey(BLE_SECURITY_PASSKEY);
        BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
#endif
        server = BLEDevice::createServer();
        server->setCallbacks(this);
        advertising = BLEDevice::getAdvertising();
        advertising->setName(BLE_DEVICE_NAME);
        battery.begin();
        robot.begin();
        uart.begin();
    }
    if (!connected) {
        startAdvertising();
    }
}

void BLE::startAdvertising() {
    if (!ble_gap_adv_active()) {
        log_i("BLE: Advertising start...");
        advertising->start(advertisingDuration);
    }
}

void BLE::stopAdvertising() {
    log_i("BLE: Advertising stop");
    advertising->stop();
}

void BLE::end() {
    log_i("BLE: Deinit");
    battery.end();
    robot.end();
    uart.end();
    BLEDevice::deinit();
    advertising = nullptr;
    server = nullptr;
    started = false;
}

void BLE::onConnect(BLEServer* bleServer, BLEConnInfo& connInfo) {
    log_i("BLE: Connected");
    stopAdvertising();
    connected = true;
}

void BLE::onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason) {
    log_i("BLE: Disconnected");
    connected = false;
}

# 1 "controller.cpp"

#include "controller.h"

#include "led.h"
#include "lora.h"
#include "ota.h"
#include "robot.h"

Controller controller;

Controller::Controller() {}

Controller::~Controller() {}

void Controller::begin() {
#if ROBOT_HAS_CONTROLLER_SERIAL
    ControllerSerial.begin(115200, SERIAL_8N1, ROBOT_CONTROLLER_SERIAL_RX_PIN, ROBOT_CONTROLLER_SERIAL_TX_PIN);
    xTaskCreate(task, "controller_task", 4096, NULL, 1, NULL);
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    ControllerSerial.begin(115200, SERIAL_8N1, ROBOT_TRANSCEIVER_SERIAL_RX_PIN, ROBOT_TRANSCEIVER_SERIAL_TX_PIN);
    xTaskCreate(task, "controller_task", 4096, NULL, 1, NULL);
#endif
}

controller_state_t Controller::getState() {
    return state;
}

void Controller::getState(uint8_t copyState[]) {
    memcpy(copyState, &state, sizeof(controller_state_t));
}

void Controller::getState(controller_state_t* copyState) {
    memcpy(copyState, &state, sizeof(controller_state_t));
}

void Controller::setState(controller_state_t newState) {
    if (memcmp(&newState, &state, sizeof(controller_state_t))) {
        controller_state_t oldState = state;
        state = newState;
        onChange(oldState);
    }
}

void Controller::setState(controller_state_t* newState) {
    setState(*newState);
}

void Controller::setState(uint8_t newState[]) {
    setState(*((controller_state_t*)newState));
}

void Controller::onChange(controller_state_t oldState) {
#if ROBOT_HAS_AUTO_MODE
    if (state.back) {
        robot.needUpdateAutoStop();
    }
    if (!state.start && oldState.start) {
        robot.needUpdateAutoStart();
    }
#endif
#if ROBOT_HAS_CHASSIS
    if (state.lx != oldState.lx || state.ly != oldState.ly || state.rx != oldState.rx || state.ry != oldState.ry || state.dx != oldState.dx) {
        robot.needUpdateSpeed();
    }
    if (state.dy != oldState.dy || state.a != oldState.a || state.b != oldState.b || state.x != oldState.x || state.y != oldState.y) {
        robot.needUpdateServo();
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_LORA
    lora.needSendControllerState();
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    needSendState();
#endif
#ifdef RGB_BUILTIN
    if (false) {
    } else if (state.a && !oldState.a) {
        led.setControllerButton('A');
    } else if (state.b && !oldState.b) {
        led.setControllerButton('B');
    } else if (state.x && !oldState.x) {
        led.setControllerButton('X');
    } else if (state.y && !oldState.y) {
        led.setControllerButton('Y');
    } else {
        led.setControllerButton(0);
    }
#endif
    if (state.back) {
        if (state.a) {
            if (state.b) {
                ota.needEnableBLE();
            }
            if (state.y) {
                ota.needEnableWiFi();
            }
        }
        if (state.x) {
            if (state.b) {
                ota.needDisableBLE();
            }
            if (state.y) {
                ota.needDisableWiFi();
            }
        }
    }
    print();
}

void Controller::print() {
    size_t length = 0;
    char message[256] = {0};
    length += snprintf(message + length, 256 - length, "[%6u][I] Controller: mode=%x ", (unsigned long)(esp_timer_get_time() / 1000ULL), state.mode);
    bool none = true;
    if (state.lx) {
        length += snprintf(message + length, 256 - length, "lx=%d ", state.lx);
        none = false;
    }
    if (state.ly) {
        length += snprintf(message + length, 256 - length, "ly=%d ", state.ly);
        none = false;
    }
    if (state.rx) {
        length += snprintf(message + length, 256 - length, "rx=%d ", state.rx);
        none = false;
    }
    if (state.ry) {
        length += snprintf(message + length, 256 - length, "ry=%d ", state.ry);
        none = false;
    }
    if (state.dx) {
        length += snprintf(message + length, 256 - length, "dx=%d ", state.dx);
        none = false;
    }
    if (state.dy) {
        length += snprintf(message + length, 256 - length, "dy=%d ", state.dy);
        none = false;
    }
    if (state.lz) {
        length += snprintf(message + length, 256 - length, "lz=%d ", state.lz);
        none = false;
    }
    if (state.rz) {
        length += snprintf(message + length, 256 - length, "rz=%d ", state.rz);
        none = false;
    }
    if (state.lt) {
        length += snprintf(message + length, 256 - length, "lt=%d ", state.lt);
        none = false;
    }
    if (state.rt) {
        length += snprintf(message + length, 256 - length, "rt=%d ", state.rt);
        none = false;
    }
    if (state.a) {
        length += snprintf(message + length, 256 - length, "a=1 ");
        none = false;
    }
    if (state.b) {
        length += snprintf(message + length, 256 - length, "b=1 ");
        none = false;
    }
    if (state.x) {
        length += snprintf(message + length, 256 - length, "x=1 ");
        none = false;
    }
    if (state.y) {
        length += snprintf(message + length, 256 - length, "y=1 ");
        none = false;
    }
    if (state.start) {
        length += snprintf(message + length, 256 - length, "start=1 ");
        none = false;
    }
    if (state.back) {
        length += snprintf(message + length, 256 - length, "back=1 ");
        none = false;
    }
    if (none) {
        length += snprintf(message + length, 256 - length, "none");
    }
    message[length] = '\n';
    length++;
    message[length] = '\0';
    log_printf(message);
}

void Controller::needSendState() {
    bool value = true;
    xQueueSend(needQueue, &value, 0);
}

void Controller::sendState() {
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    char message[32] = {0};
    uint8_t* data = (uint8_t*)(&state);
    int length = snprintf(message, 32, "CTRL=%02x%02x%02x%02x%02x%02x%02x%02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    ControllerSerial.write(message, length);
#endif
}

void Controller::task() {
#if ROBOT_HAS_CONTROLLER_SERIAL
    while (true) {
        if (ControllerSerial.available()) {
            uint8_t data[32];
            size_t length = ControllerSerial.readBytesUntil('\n', data, 32);
            if (length > 20 && strncmp((const char*)data, "CTRL=", 5) == 0) {
                data[0] = ((data[5] <= '9' ? data[5] - '0' : toupper(data[5]) - 'A' + 10) << 4) | (data[6] <= '9' ? data[6] - '0' : toupper(data[6]) - 'A' + 10);
                data[1] = ((data[7] <= '9' ? data[7] - '0' : toupper(data[7]) - 'A' + 10) << 4) | (data[8] <= '9' ? data[8] - '0' : toupper(data[8]) - 'A' + 10);
                data[2] = ((data[9] <= '9' ? data[9] - '0' : toupper(data[9]) - 'A' + 10) << 4) | (data[10] <= '9' ? data[10] - '0' : toupper(data[10]) - 'A' + 10);
                data[3] = ((data[11] <= '9' ? data[11] - '0' : toupper(data[11]) - 'A' + 10) << 4) | (data[12] <= '9' ? data[12] - '0' : toupper(data[12]) - 'A' + 10);
                data[4] = ((data[13] <= '9' ? data[13] - '0' : toupper(data[13]) - 'A' + 10) << 4) | (data[14] <= '9' ? data[14] - '0' : toupper(data[14]) - 'A' + 10);
                data[5] = ((data[15] <= '9' ? data[15] - '0' : toupper(data[15]) - 'A' + 10) << 4) | (data[16] <= '9' ? data[16] - '0' : toupper(data[16]) - 'A' + 10);
                data[6] = ((data[17] <= '9' ? data[17] - '0' : toupper(data[17]) - 'A' + 10) << 4) | (data[18] <= '9' ? data[18] - '0' : toupper(data[18]) - 'A' + 10);
                data[7] = ((data[19] <= '9' ? data[19] - '0' : toupper(data[19]) - 'A' + 10) << 4) | (data[20] <= '9' ? data[20] - '0' : toupper(data[20]) - 'A' + 10);
                setState(data);
            }
        } else {
            vTaskDelay(1);
        }
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    while (true) {
        bool update;
        if (xQueueReceive(needQueue, &update, 1000)) {
            sendState();
        }
    }
#endif
}

void Controller::task(void* arg) {
    controller.task();
}

# 1 "encoder.cpp"

#include "encoder.h"

#include "motor.h"
#include "robot.h"

Encoder encoder = Encoder(ROBOT_MOTOR_ENCODER_I2C_ADDRESS, ROBOT_MOTOR_ENCODER_INTERRUPT_PIN);

Encoder::Encoder(int8_t _i2cAddress, int8_t _interruptPin) {
    i2cAddress = _i2cAddress;
    interruptPin = _interruptPin;
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    decoder = new rotaryDecoder(_i2cAddress);
#endif
}

Encoder::~Encoder() {}

void Encoder::begin() {
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    Wire.begin(-1, -1, 400000);
#if ROBOT_MOTOR_ENCODER_INTERRUPT_PIN
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(interruptPin, interrupt, FALLING);
#endif
    decoder->begin();
    if (decoder->isConnected()) {
        log_i("Encoder: 0x%02x connected", i2cAddress);
        xTaskCreatePinnedToCore(task, "encoder_task", 4096, NULL, 1, NULL, 0);
    } else {
        log_i("Encoder: not connected");
    }
#endif
}

void Encoder::task() {
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    decoder->reset();
    int count = 0;
    int debug = 0;
    while (true) {
        if (needUpdate) {
            log_i("Encoder: need update");
            needUpdate = false;
        }
        decoder->update();
        if (++count > 1024) {
            count = 0;
            bool updateSpeed = false;
            int value1 = decoder->getValue(0);
            if (-4 < value1 && value1 < 4) {
                value1 = 0;
            }
            if (motorLF.getEncoderSpeed() != value1) {
                motorLF.setEncoderSpeed(value1);
                updateSpeed = true;
            }
            int value2 = decoder->getValue(1);
            if (-4 < value2 && value2 < 4) {
                value2 = 0;
            }
            if (motorRF.getEncoderSpeed() != value2) {
                motorRF.setEncoderSpeed(value2);
                updateSpeed = true;
            }
            int value3 = decoder->getValue(2);
            if (-4 < value3 && value3 < 4) {
                value3 = 0;
            }
            if (motorLB.getEncoderSpeed() != value3) {
                motorLB.setEncoderSpeed(value3);
                updateSpeed = true;
            }
            int value4 = decoder->getValue(3);
            if (-4 < value4 && value4 < 4) {
                value4 = 0;
            }
            if (motorRB.getEncoderSpeed() != value4) {
                motorRB.setEncoderSpeed(value4);
                updateSpeed = true;
            }
            if (updateSpeed) {
                robot.needUpdateSpeed();
            }
            decoder->reset();
            if (++debug > 32) {
                debug = 0;
                log_i("Encoder: %d %d %d %d", value1, value2, value3, value4);
            }
        }
    }
#endif
}

void Encoder::task(void* arg) {
    encoder.task();
}

void Encoder::interrupt() {
    encoder.needUpdate = true;
}

# 1 "imu.cpp"

#include "imu.h"

IMU imu;

IMU::IMU() {
#if ROBOT_HAS_IMU
    bmx = new iarduino_Position_BMX055(BMX);
#endif
}

IMU::~IMU() {}

void IMU::begin() {
    if (bmx) {
        if (bmx->begin(&Wire)) {
            log_i("Gyroscope: connected");
            xTaskCreatePinnedToCore(task, "imu_task", 4096, NULL, 1, &startedTask, 0);
        } else {
            log_i("Gyroscope: not connected");
        }
    }
}

int IMU::getAxisX() {
    if (bmx) {
        return bmx->axisX;
    } else {
        return 0;
    }
}

int IMU::getAxisY() {
    if (bmx) {
        return bmx->axisY;
    } else {
        return 0;
    }
}

int IMU::getAxisZ() {
    if (bmx) {
        return bmx->axisZ;
    } else {
        return 0;
    }
}

void IMU::calibrate(int time) {
    float coefficients[3] = { 0, 0, 0 };
    for (int n = 0; n < 10; n ++) {
        vTaskDelay(1000);
        log_i("Gyroscope: calibrate");
        unsigned long i = millis();
        while ((millis() - i) < time) {
            bmx->setFastOffset(BMM);
            vTaskDelay(1);
        }
        bmx->getFastOffset(coefficients);
        log_i("float coefficients[3] = { %f, %f, %f }", coefficients[0], coefficients[1], coefficients[2]);
    }
}

void IMU::task() {
    bmx->setScale(BMA_8G);
    bmx->setScale(BMG_500DPS);
    bmx->setScale(BMM_HIGH);
    bmx->setBandwidths(BMA_63Hz);
    bmx->setBandwidths(BMG_64Hz);
    bmx->setBandwidths(BMM_20Hz);
    bmx->setFastOffset(BMA);
    bmx->setFastOffset(BMG);
    // calibrate();
    float coefficients[3] = { -179.375000, -116.875000, -81.875000 };
    bmx->setFastOffset(coefficients);
    int count = 0;
    int debug = 0;
    while (true) {
        if (!bmx->read()) {
            vTaskDelay(1);
            continue;
        }
        /*
        if (++count > 10) {
            count = 0;
            if (++debug > 10) {
                debug = 0;
                log_i("Gyroscope: x=%d, y=%d, z=%d", (int)bmx->axisX, (int)bmx->axisY, (int)bmx->axisZ);
            }
        }
        */
    }
}

void IMU::task(void* arg) {
    imu.task();
}

# 1 "led.cpp"

#include "led.h"

Led led;

Led::Led() {}

Led::~Led() {}

void Led::begin() {
    setPowerOn(true);
#ifdef RGB_BUILTIN
    rgbPin = RGB_BUILTIN;
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
#else
#ifdef LED_BUILTIN
    ledPin = LED_BUILTIN;
    ledcAttach(LED_BUILTIN, 5000, 8);
#endif
#endif
    xTaskCreate(task, "led_task", 4096, NULL, 1, NULL);
}

void Led::onChange() {
#ifdef RGB_BUILTIN
    uint8_t r = 0x00, g = 0x00, b = 0x00;
    if (timers.power.on.value) {
        r = g = b = 0x33;
    } else if (timers.ota.ble.value) {
        g = b = 0x11;
    } else if (timers.ota.wifi.value) {
        g = b = 0x11;
    } else if (state.controller.button == 'A') {
        g = 0x11;
    } else if (state.controller.button == 'B') {
        r = 0x11;
    } else if (state.controller.button == 'X') {
        b = 0x11;
    } else if (state.controller.button == 'Y') {
        r = g = 0x11;
    }
    rgbLedWrite(rgbPin, r, g, b);
#else
#ifdef LED_BUILTIN
    uint8_t c = 0x00;
    if (timers.power.on.value) {
        c = 0x11;
    } else if (timers.lora.errors.value) {
        c = 0x01;
    } else if (timers.ota.ble.value) {
        c = 0x01;
    } else if (timers.ota.wifi.value) {
        c = 0x01;
    }
    if (state.lora.sending) {
        c++;
    } else if (state.lora.sleeping) {
        c = timers.lora.sleeping.value ? 1 : 0;
    }
    ledcWrite(ledPin, c);
#endif
#endif
}

void Led::setPowerOn(bool value) {
    if (state.power.on != value) {
        timers.power.on.counter = 0;
        timers.power.on.value = false;
        state.power.on = value;
        changed = true;
    }
}

void Led::setControllerButton(char value) {
    if (state.controller.button != value) {
        state.controller.button = value;
        changed = true;
    }
}

void Led::setLoraErrors(bool value) {
    if (state.lora.errors != value) {
        timers.lora.sleeping.counter = 0;
        timers.lora.sleeping.value = false;
        state.lora.errors = value;
        changed = true;
    }
}

void Led::setLoraSending(bool value) {
    if (state.lora.sending != value) {
        timers.lora.errors.counter = 0;
        timers.lora.errors.value = false;
        timers.lora.sleeping.counter = 0;
        timers.lora.sleeping.value = false;
        state.lora.sending = value;
        changed = true;
    }
}

void Led::setLoraSleeping(bool value) {
    if (state.lora.sleeping != value) {
        timers.lora.errors.counter = 0;
        timers.lora.errors.value = false;
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
        state.lora.sleeping = value;
        changed = true;
    }
}

void Led::setOtaBLE(bool value) {
    if (state.ota.ble != value) {
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
        state.ota.ble = value;
        changed = true;
    }
}

void Led::setOtaWiFi(bool value) {
    if (state.ota.wifi != value) {
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        state.ota.wifi = value;
        changed = true;
    }
}

void Led::task() {
    while (true) {
        if (state.power.on) {
            switch (++timers.power.on.counter % timers.power.on.period) {
                case 1:
                    timers.power.on.value = true;
                    changed = true;
                    break;
                case 0:
                    timers.power.on.value = false;
                    state.power.on = false;
                    changed = true;
                    break;
            }
        }
        if (state.lora.errors) {
            switch (++timers.lora.errors.counter % timers.lora.errors.period) {
                case 0:
                    timers.lora.errors.value = !timers.lora.errors.value;
                    changed = true;
                    break;
            }
        }
        if (state.lora.sleeping) {
            switch (++timers.lora.sleeping.counter % timers.lora.sleeping.period) {
                case 0:
                    timers.lora.sleeping.value = !timers.lora.sleeping.value;
                    changed = true;
                    break;
            }
        }
        if (state.ota.ble) {
            switch (++timers.ota.ble.counter % timers.ota.ble.period) {
                case 1:
                    timers.ota.ble.value = true;
                    changed = true;
                    break;
                case 11:
                    timers.ota.ble.value = false;
                    changed = true;
                    break;
            }
        }
        if (state.ota.wifi) {
            switch (++timers.ota.wifi.counter % timers.ota.wifi.period) {
                case 21:
                case 41:
                    timers.ota.wifi.value = true;
                    changed = true;
                    break;
                case 31:
                case 51:
                    timers.ota.wifi.value = false;
                    changed = true;
                    break;
            }
        }
        if (changed) {
            changed = false;
            onChange();
        }
        counter++;
        vTaskDelay(10);
    }
}

void Led::task(void* arg) {
    led.task();
}

# 1 "lidar.cpp"

#include "lidar.h"

Lidar::Lidar() {}

Lidar::~Lidar() {}

void Lidar::begin() {
    pinMode(ROBOT_LIDAR_RX_PIN, INPUT);
    pinMode(ROBOT_LIDAR_TX_PIN, OUTPUT);
    LidarSerial.setTimeout(100);
    LidarSerial.begin(460800);
    zero();
}

bool Lidar::start() {
    if (startedTask) {
        return false;
    }
    needStop = false;
    xTaskCreatePinnedToCore(task, "lidar_task", 8192, NULL, 1, &startedTask, 0);
    return true;
}

bool Lidar::stop() {
    if (needStop) {
        return false;
    }
    needStop = true;
    return true;
}

void Lidar::addRoadObject(road_object_t object, road_object_t* objects, int& objectCount, int objectCountMax) {
    int index = -1;
    for (int n = 0; n < objectCount; n++) {
        if (objects[n].angle0 <= object.angle0 && object.angle1 <= objects[n].angle1) {
            return;
        } else if (object.angle0 <= objects[n].angle0 && objects[n].angle1 <= object.angle1) {
            index = n;
            break;
        } else if (objects[n].angle0 <= object.angle0 && object.angle0 <= objects[n].angle1) {
            object.angle0 = objects[n].angle0;
            object.distance0 = objects[n].distance0;
            object.distance = min(objects[n].distance, object.distance);
            index = n;
        } else if (objects[n].angle0 <= object.angle1 && object.angle1 <= objects[n].angle1) {
            object.angle1 = objects[n].angle1;
            object.distance1 = objects[n].distance1;
            object.distance = min(objects[n].distance, object.distance);
            index = n;
        }
    }
    object.angle = object.angle1 - object.angle0 + 1;
    if (0 < object.angle && object.angle < 90) {
        object.width = sqrt(object.distance0 * object.distance0 + object.distance1 * object.distance1 - ((object.distance0 * object.distance1 * cosines[object.angle]) >> 11));
        if (10 < object.width && object.width < 90) {
            if (index < 0) {
                if (objectCount < objectCountMax) {
                    index = objectCount;
                    objectCount++;
                } else {
                    return;
                }
            }
            objects[index] = object;
        }
    }
}

void Lidar::scanRoadObjects(road_object_t* objects, int& objectCount, int objectCountMax) {
    road_object_t object;
    int distancePrev = 0;
    bool isObject = false;
    for (int n = 270; n <= 450; n++) {
        int angle = n % 360;
        int distance = distances[angle];
        if (0 < distance && distance < 1000) {
            if (isObject) {
                if (abs(distancePrev - distance) < 100) {
                    object.angle1 = n;
                    object.distance1 = distance;
                    object.distance = min(distance, object.distance);
                } else {
                    isObject = false;
                    addRoadObject(object, objects, objectCount, objectCountMax);
                }
            } else {
                isObject = true;
                object.angle0 = object.angle1 = n;
                object.distance0 = object.distance1 = object.distance = distance;
            }
        } else {
            if (isObject) {
                isObject = false;
                addRoadObject(object, objects, objectCount, objectCountMax);
            }
        }
        distancePrev = distance;
    }
}

void Lidar::task() {
    int errors = 0;
    while (!sendStartCommand()) {
        vTaskDelay(1000);
    }
    zero();
    while (!needStop) {
        // 0..359
        uint16_t angle = 0;
        // 0..
        uint16_t distance = 0;
        // 0..
        uint8_t strength = 0;
        // Сканирование
        if (scan(angle, distance, strength)) {
            distances[angle] = distance;
            errors = 0;
        } else if (++errors > 999) {
            log_e("Lidar: %d errors\n", errors);
            sendStopCommand();
            vTaskDelay(1000);
            sendStartCommand();
            errors = 0;
        } else {
            vTaskDelay(1);
        }
    }
    zero();
    sendStopCommand();
    startedTask = nullptr;
    vTaskDelete(NULL);
}

bool Lidar::scan(uint16_t& angle, uint16_t& distance, uint8_t& strength) {
    if (!startedTask) {
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

bool Lidar::sendStartCommand() {
    skipAll();
    if (!sendCommand(0x20)) {
        skipAll();
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
    return true;
}

bool Lidar::sendResetCommand() {
    skipAll();
    if (!sendCommand(0x40)) {
        skipAll();
        return false;
    }
    skipAll();
    return true;
}

bool Lidar::sendStopCommand() {
    skipAll();
    if (!sendCommand(0x25)) {
        skipAll();
        return false;
    }
    skipAll();
    return true;
}

bool Lidar::getDeviceInfo() {
    if (startedTask) {
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

bool Lidar::getDeviceHealth() {
    if (startedTask) {
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

bool Lidar::getLidarConf(uint32_t conf) {
    if (startedTask) {
        return false;
    }
    if (!sendCommand(0x84, (uint8_t*)(&conf), 4)) {
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

bool Lidar::sendCommand(uint8_t command, uint8_t* payload, uint8_t size) {
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

bool Lidar::readHeader(uint8_t& type, uint8_t& subtype, uint32_t& length) {
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

bool Lidar::skipBytes(size_t length) {
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

bool Lidar::skipAll() {
    uint8_t c;
    while (LidarSerial.available()) {
        LidarSerial.readBytes(&c, 1);
    }
    return true;
}

void Lidar::zero() {
    memset(distances, 0, sizeof(distances));
}

void Lidar::task(void* arg) {
    lidar.task();
}

Lidar lidar = Lidar();

# 1 "lora.cpp"

#include "lora.h"

#include "controller.h"
#include "led.h"
#include "tea.h"

// https://github.com/jgromes/RadioLib
// https://github.com/ropg/heltec_esp32_lora_v3

Lora lora;

Lora::Lora() {
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    module = new SX1262(new Module(SS, DIO1, RST_LoRa, BUSY_LoRa));
#endif
}

Lora::~Lora() {
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    delete module;
#endif
}

void Lora::begin() {
    if (!module) {
        return;
    }
    module->begin(868.f, 125.f, 7, 5, 0xfa, 16);
#if ROBOT_HAS_TRANSCEIVER_LORA
    module->setPacketSentAction(packetSentCallback);
#endif
#if ROBOT_HAS_CONTROLLER_LORA
    module->setPacketReceivedAction(packetReceivedCallback);
    module->startReceive();
#endif
    if (!startedTask) {
        xTaskCreate(task, "lora_task", 4096, NULL, 1, &startedTask);
    }
}

void Lora::onPacketSent() {
    doneSend = true;
}

void Lora::onPacketReceived() {
    needRead = true;
}

void Lora::needSendControllerState() {
    needSend = true;
}

bool Lora::readControllerState() {
    int length = module->getPacketLength();
    if (length == 8) {
        uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        module->readData(data, length);
        log_d("%02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        if (tea.decryptData(data)) {
            log_d("%02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            controller.setState(data);
            return true;
        }
    }
    return false;
}

bool Lora::sendControllerState() {
    uint8_t data[8];
    controller.getState(data);
    tea.encryptData(data);
    return module->startTransmit(data, 8) == RADIOLIB_ERR_NONE;
}

void Lora::reset() {
    module->reset();
    begin();
}

void Lora::task() {
#if ROBOT_HAS_CONTROLLER_LORA
    while (true) {
        if (needRead) {
            needRead = false;
            if (readControllerState()) {
                errors = 0;
            } else {
                errors++;
                if (errors > 9) {
                    errors = 0;
                    reset();
                }
            }
        } else {
            vTaskDelay(1);
        }
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_LORA
    while (true) {
        if (sending) {
            if (doneSend) {
                sending = false;
                doneSend = false;
                module->finishTransmit();
                led.setLoraSending(false);
                delays = 0;
                vTaskDelay(1);
            } else {
                vTaskDelay(1);
            }
        } else if (needSend) {
            needSend = false;
            if (sleeping) {
                sleeping = false;
                if (module->standby() != RADIOLIB_ERR_NONE) {
                    module->reset();
                    begin();
                }
                led.setLoraSleeping(false);
            }
            sending = true;
            led.setLoraSending(true);
            if (sendControllerState()) {
                errors = 0;
            } else {
                errors++;
                if (errors > 9) {
                    errors = 0;
                    reset();
                }
            }
            delays = 0;
            vTaskDelay(1);
        } else if (sleeping) {
            vTaskDelay(100);
        } else if (++delays >= 99999) {
            sleeping = true;
            module->sleep(true);
            led.setLoraSleeping(true);
            vTaskDelay(100);
        } else {
            vTaskDelay(1);
        }
    }
#endif
}

void Lora::packetSentCallback() {
    lora.onPacketSent();
}

void Lora::packetReceivedCallback() {
    lora.onPacketReceived();
}

void Lora::task(void* arg) {
    lora.task();
}

# 1 "main.cpp"

#include "controller.h"
#include "led.h"
#include "ota.h"
#include "robot.h"
#include "settings.h"
#include "version.h"
#if ROBOT_HAS_CONTROLLER_USB
#include "usb.h"
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
#include "lora.h"
#endif
#if ROBOT_HAS_LIDAR
#include "lidar.h"
#endif
#if ROBOT_HAS_MOTOR_ENCODER_I2C
#include "encoder.h"
#endif
#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
#include "navigation.h"
#endif
#if ROBOT_HAS_IMU
#include "imu.h"
#endif

void setup() {
    Serial.begin(115200);
    log_i("%s (%s.local) Firmware: %u (%s)", BLE_DEVICE_NAME, NET_HOSTNAME, BUILD_TIMESTAMP, BUILD_DATETIME);
    settings.begin();
#if ROBOT_HAS_CONTROLLER_USB
    usb.begin();
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    lora.begin();
#endif
#if ROBOT_HAS_CONTROLLER_SERIAL || ROBOT_HAS_TRANSCEIVER_SERIAL
    controller.begin();
#endif
#if ROBOT_HAS_LIDAR
    lidar.begin();
#endif
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    encoder.begin();
#endif
#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
    navigation.begin();
#endif
#if ROBOT_HAS_IMU
    imu.begin();
#endif
#ifdef RGB_BUILTIN
    led.begin();
#else
#ifdef LED_BUILTIN
    led.begin();
#endif
#endif
    ota.begin();
    robot.begin();
}

void loop() {
    vTaskDelay(1000);
}

# 1 "motor.cpp"

#include "motor.h"

#if ROBOT_HAS_MOTOR_MCPWM
MotorMCPWM motorLF = MotorMCPWM("LF", ROBOT_MOTOR_LF_PIN1, ROBOT_MOTOR_LF_PIN2);
MotorMCPWM motorRF = MotorMCPWM("RF", ROBOT_MOTOR_RF_PIN1, ROBOT_MOTOR_RF_PIN2);
MotorMCPWM motorLB = MotorMCPWM("LB", ROBOT_MOTOR_LB_PIN1, ROBOT_MOTOR_LB_PIN2);
MotorMCPWM motorRB = MotorMCPWM("RB", ROBOT_MOTOR_RB_PIN1, ROBOT_MOTOR_RB_PIN2);
#elif ROBOT_HAS_MOTOR_PWM
MotorPWM motorLF = MotorPWM("LF", ROBOT_MOTOR_LF_PIN1, ROBOT_MOTOR_LF_PIN2);
MotorPWM motorRF = MotorPWM("RF", ROBOT_MOTOR_RF_PIN1, ROBOT_MOTOR_RF_PIN2);
MotorPWM motorLB = MotorPWM("LB", ROBOT_MOTOR_LB_PIN1, ROBOT_MOTOR_LB_PIN2);
MotorPWM motorRB = MotorPWM("RB", ROBOT_MOTOR_RB_PIN1, ROBOT_MOTOR_RB_PIN2);
#else
Motor motorLF = Motor("LF");
Motor motorRF = Motor("RF");
Motor motorLB = Motor("LB");
Motor motorRB = Motor("RB");
#endif

#if ROBOT_HAS_MOTOR_1_MCPWM
MotorMCPWM motor1 = MotorMCPWM("1", ROBOT_MOTOR_1_PIN1, ROBOT_MOTOR_1_PIN2);
#elif ROBOT_HAS_MOTOR_1_PWM
MotorPWM motor1 = MotorPWM("1", ROBOT_MOTOR_1_PIN1, ROBOT_MOTOR_1_PIN2);
#else
Motor motor1 = Motor("1");
#endif
#if ROBOT_HAS_MOTOR_2_MCPWM
MotorMCPWM motor2 = MotorMCPWM("2", ROBOT_MOTOR_2_PIN1, ROBOT_MOTOR_2_PIN2);
#elif ROBOT_HAS_MOTOR_2_PWM
MotorPWM motor2 = MotorPWM("2", ROBOT_MOTOR_2_PIN1, ROBOT_MOTOR_2_PIN2);
#else
Motor motor2 = Motor("2");
#endif
#if ROBOT_HAS_MOTOR_3_MCPWM
MotorMCPWM motor3 = MotorMCPWM("3", ROBOT_MOTOR_3_PIN1, ROBOT_MOTOR_3_PIN2);
#elif ROBOT_HAS_MOTOR_3_PWM
MotorPWM motor3 = MotorPWM("3", ROBOT_MOTOR_3_PIN1, ROBOT_MOTOR_3_PIN2);
#else
Motor motor3 = Motor("3");
#endif

Motor::Motor(const char* _name) {
    name = _name;
}

Motor::~Motor() {}

void Motor::begin() {}

const char* Motor::getName() {
    return name;
}

void Motor::setMinSpeed(uint value) {
    minSpeed = value;
}

void Motor::setMaxSpeed(uint value) {
    maxSpeed = value;
}

int Motor::getEncoderSpeed() {
    return encoderSpeed;
}

void Motor::setEncoderSpeed(int value) {
    encoderSpeed = value;
    if (value) {
        encoderWorks = true;
    }
}

void Motor::setSpeed(int value) {
    bool back = value < 0;
    uint absSpeed = back ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < minSpeed) {
                absSpeed = minSpeed;
            } else if (absSpeed > maxSpeed) {
                absSpeed = maxSpeed;
            }
        }
    }
    int newSpeed = back ? -absSpeed : absSpeed;
    if (speed != newSpeed) {
        speed = newSpeed;
        log_i("Motor %s: %d", name, speed);
    }
}

MotorPWM::MotorPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2) : Motor(_name) {
    pwmPin1 = _pwmPin1;
    pwmPin2 = _pwmPin2;
}

MotorPWM::~MotorPWM() {}

void MotorPWM::begin() {
    if (pwmPin1) {
        ledcAttach(pwmPin1, 25000, 8);
    }
    if (pwmPin2) {
        ledcAttach(pwmPin2, 25000, 8);
    }
}

void MotorPWM::setSpeed(int value) {
    bool back = value < 0;
    uint absSpeed = back ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < 0xff) {
                absSpeed = minSpeed + absSpeed * (maxSpeed - minSpeed) / 0xff;
            } else {
                absSpeed = maxSpeed;
            }
        }
    }
    int newSpeed = back ? -absSpeed : absSpeed;
    if (speed != newSpeed) {
        speed = newSpeed;
        if (pwmPin1 && pwmPin2) {
            ledcWrite(pwmPin1, back ? 0 : absSpeed);
            ledcWrite(pwmPin2, back ? absSpeed : 0);
        }
        log_i("Motor %s: %d", name, speed);
    }
}

MotorMCPWM::MotorMCPWM(const char* _name, uint8_t _pwmPin1, uint8_t _pwmPin2) : MotorPWM(_name, _pwmPin1, _pwmPin2) {
    if (strlen(name) > 1) {
        isLeft = name[0] == 'L';
        isRight = name[0] == 'R';
        isCenter = name[0] == 'C';
        isFront = name[1] == 'F';
        isBack = name[1] == 'B';
    }
}

MotorMCPWM::~MotorMCPWM() {}

void MotorMCPWM::begin() {
    int group = isFront ? 0 : 1;
    mcpwm_timer_config_t timerConfig = {
        .group_id = group,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000000 / 25000,
    };
    if (strlen(name) == 1) {
        // 1, 2, 3, 4
        maxSpeed = timerConfig.period_ticks;
    } else {
        // LF, RF, LB, RB, CC
        maxSpeed = 0.8f * timerConfig.period_ticks;
    }
    minSpeed = 0.4f * timerConfig.period_ticks;
    mcpwm_new_timer(&timerConfig, &mcpwmTimer);
    mcpwm_operator_config_t operatorConfig = {
        .group_id = group,
    };
    mcpwm_new_operator(&operatorConfig, &mcpwmOperator);
    mcpwm_operator_connect_timer(mcpwmOperator, mcpwmTimer);
    mcpwm_comparator_config_t comparatorConfig = {};
    mcpwm_new_comparator(mcpwmOperator, &comparatorConfig, &mcpwmComparator1);
    mcpwm_new_comparator(mcpwmOperator, &comparatorConfig, &mcpwmComparator2);
    mcpwm_comparator_set_compare_value(mcpwmComparator1, 0);
    mcpwm_comparator_set_compare_value(mcpwmComparator2, 0);
    mcpwm_generator_config_t generatorConfig = {};
    generatorConfig.gen_gpio_num = pwmPin1 ? pwmPin1 : -1;
    mcpwm_new_generator(mcpwmOperator, &generatorConfig, &mcpwmGenerator1);
    generatorConfig.gen_gpio_num = pwmPin2 ? pwmPin2 : -1;
    mcpwm_new_generator(mcpwmOperator, &generatorConfig, &mcpwmGenerator2);
    mcpwm_generator_set_action_on_timer_event(mcpwmGenerator1, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(mcpwmGenerator1, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwmComparator1, MCPWM_GEN_ACTION_LOW));
    mcpwm_generator_set_action_on_timer_event(mcpwmGenerator2, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(mcpwmGenerator2, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwmComparator2, MCPWM_GEN_ACTION_LOW));
    mcpwm_timer_enable(mcpwmTimer);
    mcpwm_timer_start_stop(mcpwmTimer, MCPWM_TIMER_START_NO_STOP);
    mcpwm_generator_set_force_level(mcpwmGenerator1, 0, true);
    mcpwm_generator_set_force_level(mcpwmGenerator2, 0, true);
}

void MotorMCPWM::setSpeed(int value) {
    uint absSpeed = value < 0 ? -value : value;
    if (absSpeed) {
        if (minSpeed < maxSpeed) {
            if (absSpeed < 0xff) {
                absSpeed = minSpeed + absSpeed * (maxSpeed - minSpeed) / 0xff;
            } else {
                absSpeed = maxSpeed;
            }
        }
    }
    int newSpeed = value < 0 ? -absSpeed : absSpeed;
    if (speed != newSpeed) {
        mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator1 : mcpwmGenerator2, value < 0 ? -1 : 0, true);
        mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator2 : mcpwmGenerator1, value > 0 ? -1 : 0, true);
        speed = newSpeed;
        mcpwm_comparator_set_compare_value(mcpwmComparator1, absSpeed);
        mcpwm_comparator_set_compare_value(mcpwmComparator2, absSpeed);
        log_i("Motor %s: %d", name, speed);
    } else if (speed && encoderWorks) {
        int change = 0;
        float ratio = 0;
        if (!encoderSpeed) {
            if (absSpeed + increaseSpeed < maxSpeed) {
                increaseSpeed ++;
                absSpeed += increaseSpeed;
                newSpeed = value < 0 ? -absSpeed : absSpeed;
                change = 1;
            }
        } else {
            ratio = -(float)encoderSpeed / speed;
            if (0.0 < ratio && ratio < 8.0) {
                if (absSpeed + increaseSpeed < maxSpeed) {
                    increaseSpeed ++;
                    absSpeed += increaseSpeed;
                    newSpeed = value < 0 ? -absSpeed : absSpeed;
                    change = 2;
                }
            } else if (12.0 < ratio && ratio < 20.0) {
                if (increaseSpeed > 0) {
                    increaseSpeed --;
                    if (increaseSpeed) {
                        absSpeed += increaseSpeed;
                        newSpeed = value < 0 ? -absSpeed : absSpeed;
                        change = 3;
                    }
                }
            }
        }
        if (change) {
            /*
            mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator1 : mcpwmGenerator2, value < 0 ? -1 : 0, true);
            mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator2 : mcpwmGenerator1, value > 0 ? -1 : 0, true);
            mcpwm_comparator_set_compare_value(mcpwmComparator1, absSpeed);
            mcpwm_comparator_set_compare_value(mcpwmComparator2, absSpeed);
            */
            log_i("Motor %s: %d -> %d (encoder %d, ratio %d%%, reason %d)", name, speed, newSpeed, -encoderSpeed, (int)(ratio * 100), change);
        }
    }
}

# 1 "navigation.cpp"

#include "navigation.h"

#include "controller.h"

Navigation navigation;

Navigation::Navigation() {}

Navigation::~Navigation() {}

void Navigation::begin() {
#if (ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL) && !(ROBOT_HAS_CONTROLLER_SERIAL || ROBOT_HAS_TRANSCEIVER_SERIAL)
    NavigationSerial.begin(115200, SERIAL_8N1, 19, 20);
#endif
#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
    xTaskCreate(task, "navigation_task", 4096, NULL, 1, NULL);
#endif
}

void Navigation::task() {
#if ROBOT_HAS_NAVIGATION_SENDER
    while (true) {
        vTaskDelay(1000);
    }
#endif
#if ROBOT_HAS_NAVIGATION_SERIAL
    while (true) {
        if (NavigationSerial.available()) {
            uint8_t data[32];
            size_t length = NavigationSerial.readBytesUntil('\n', data, 31);
            data[length] = '\0';
            Serial.print((char*)data);
        } else {
            vTaskDelay(1000);
        }
    }
#endif
}

void Navigation::task(void* arg) {
    navigation.task();
}

# 1 "ota.cpp"

#include "ota.h"

#include "ble.h"
#include "led.h"
#include "settings.h"

OTA ota;

OTA::OTA() {}

OTA::~OTA() {}

void OTA::begin() {
    ArduinoOTA.setHostname(NET_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    if (!startedTask) {
        xTaskCreate(task, "ota_task", 16384, NULL, 1, &startedTask);
        int8_t value = settings.getOtaMode();
        xQueueSend(needQueue, &value, 0);
    }
}

void OTA::beginBLE() {
    if (otaMode & OTA_WIFI) {
        otaMode = OTA_ALL;
    } else {
        otaMode = OTA_BLE;
    }
    ble.begin();
    led.setOtaBLE(true);
}

void OTA::beginWiFi() {
    if (otaMode & OTA_BLE) {
        otaMode = OTA_ALL;
    } else {
        otaMode = OTA_WIFI;
    }
    wifiMode = settings.getWiFiMode();
    String ssid = settings.getWiFiSSID();
    String password = settings.getWiFiPassword();
    if (wifiMode == WIFI_MODE_STA) {
        log_i("Wi-Fi: STA %s", ssid.c_str());
        WiFi.setHostname(NET_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        log_i("Wi-Fi: AP %s", ssid.c_str());
        WiFi.softAPsetHostname(NET_HOSTNAME);
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
        log_i("Wi-Fi: Enabled: %s", WiFi.softAPIP().toString().c_str());
    }
}

void OTA::enableBLE() {
    settings.addOtaBLE();
    beginBLE();
}

void OTA::enableWiFi() {
    settings.addOtaWiFi();
    beginWiFi();
}

void OTA::disableBLE() {
    settings.removeOtaBLE();
    endBLE();
}

void OTA::disableWiFi() {
    settings.removeOtaWiFi();
    endWiFi();
}

void OTA::needEnableBLE() {
    int8_t value = OTA_BLE;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needEnableWiFi() {
    int8_t value = OTA_WIFI;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needDisableBLE() {
    int8_t value = -OTA_BLE;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needDisableWiFi() {
    int8_t value = -OTA_WIFI;
    xQueueSend(needQueue, &value, 0);
}

void OTA::endBLE() {
    ble.end();
    led.setOtaBLE(false);
    if (otaMode & OTA_WIFI) {
        otaMode = OTA_WIFI;
    } else {
        otaMode = OTA_OFF;
    }
}

void OTA::endWiFi() {
    log_i("Wi-Fi: Disconnect");
    if (wifiMode == WIFI_MODE_STA) {
        WiFi.setAutoReconnect(false);
        WiFi.disconnect(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        WiFi.softAPdisconnect(true);
    }
    wifiMode = WIFI_MODE_NULL;
    led.setOtaWiFi(false);
    if (otaMode & OTA_BLE) {
        otaMode = OTA_BLE;
    } else {
        otaMode = OTA_OFF;
    }
}

void OTA::task() {
    if (xQueueReceive(needQueue, &otaMode, 1000)) {
        switch (otaMode) {
            case OTA_OFF:
                vTaskDelay(1000);
                break;
            case OTA_BLE:
                vTaskDelay(1000);
                beginBLE();
                vTaskDelay(1000);
                break;
            case OTA_WIFI:
                vTaskDelay(1000);
                beginWiFi();
                vTaskDelay(1000);
                break;
            case OTA_ALL:
                vTaskDelay(1000);
                beginBLE();
                vTaskDelay(1000);
                beginWiFi();
                vTaskDelay(1000);
                break;
        }
    }
    while (true) {
        int8_t action;
        if (xQueueReceive(needQueue, &action, 1000)) {
            if (!action) {
            } else if (action == OTA_BLE) {
                enableBLE();
                vTaskDelay(1000);
            } else if (action == -OTA_BLE) {
                disableBLE();
                vTaskDelay(1000);
            } else if (action == OTA_WIFI) {
                enableWiFi();
                vTaskDelay(1000);
            } else if (action == -OTA_WIFI) {
                disableWiFi();
                vTaskDelay(1000);
            }
            if (ble.robot.otaMode.characteristic) {
                ble.robot.otaMode.characteristic->setValue(otaMode);
            }
        }
        if (wifiMode == WIFI_MODE_STA) {
            if (wifiStatus != WiFi.status()) {
                wifiStatus = WiFi.status();
                if (wifiStatus == WL_CONNECTED) {
                    log_i("Wi-Fi: Connected: %s", WiFi.localIP().toString().c_str());
                    wifiConnected = true;
                    ArduinoOTA.begin();
                } else {
                    if (wifiConnected) {
                        log_i("Wi-Fi: Disconnected");
                        wifiConnected = false;
                        ArduinoOTA.end();
                    }
                }
            }
            if (wifiConnected) {
                ArduinoOTA.handle();
            }
            vTaskDelay(1000);
        } else if (wifiMode == WIFI_MODE_AP) {
            ArduinoOTA.handle();
            vTaskDelay(1000);
        } else {
            vTaskDelay(1000);
        }
    }
}

void OTA::task(void* arg) {
    ota.task();
}

# 1 "robot.cpp"

#include "robot.h"

#include "controller.h"
#include "encoder.h"
#include "imu.h"
#include "lidar.h"
#include "motor.h"
#include "servo.h"

Robot robot;

Robot::Robot() {}

Robot::~Robot() {}

void Robot::begin() {
    if (!startedTask) {
        xTaskCreate(task, "robot_task", 4096, NULL, 1, &startedTask);
    }
}

void Robot::setSpeed(int speedLF, int speedRF, int speedLB, int speedRB) {
    motorLF.setSpeed(speedLF);
    motorRF.setSpeed(speedRF);
    motorLB.setSpeed(speedLB);
    motorRB.setSpeed(speedRB);
}

void Robot::autoStart() {
    if (!autoStartedTask) {
        xTaskCreate(autoTask, "robot_auto_task", 4096, NULL, 1, &autoStartedTask);
    }
}

void Robot::autoStop() {
    if (autoStartedTask) {
        log_i("Robot: auto stop");
        vTaskDelete(autoStartedTask);
        autoStartedTask = nullptr;
        autoEnd();
    }
}

void Robot::autoEnd() {
    log_i("Robot: auto end");
#if ROBOT_HAS_LIDAR
    lidar.stop();
#endif
    controller_state_t state = controller.getState();
    state.lx = 0;
    state.ly = 0;
    state.rx = 0;
    state.ry = 0;
    controller.setState(state);
}

void Robot::updateSpeed() {
    controller_state_t state = controller.getState();

    int ly = (int)36 * state.ly;
    int lx = (int)36 * state.lx;
    int ry = (int)30 * state.ry;
    int rx = (int)30 * state.rx;

    if (abs(ry) > abs(rx) && ((ry > 0 && ly < 0) || (ry < 0 && ly > 0))) {
        // Разворот
        int ly = (int)30 * state.ly;
        int ry = (int)30 * state.ry;
        setSpeed(ly, ry, ly, ry);
    } else {
        // Обычное
        setSpeed(ly + lx + rx, ly - lx - rx, ly - lx + rx, ly + lx - rx);
    }

    // (state.dy == 0) {
        if (state.ly != 0) {
            if (wheelDown) {
                motor1.setSpeed(ly + (rx >> 2));
                motor2.setSpeed(ly - (rx >> 2));
            } else {
                motor1.setSpeed(0);
                motor2.setSpeed(0);
            }
        } else {
            motor1.setSpeed(0);
            motor2.setSpeed(0);
        }
    //     if (state.dx == 0) {
    //         motor3.setSpeed(0);
    //     } else if (state.dx > 0) {
    //         motor3.setSpeed(255);
    //     } else if (state.dx < 0) {
    //         motor3.setSpeed(-255);
    //     }
    // } else if (state.dy > 0) {
    //     if (state.dx == 0) {
    //         motor1.setSpeed(255);
    //         motor2.setSpeed(255);
    //     } else if (state.dx > 0) {
    //         motor1.setSpeed(255);
    //         motor2.setSpeed(240);
    //     } else if (state.dx < 0) {
    //         motor1.setSpeed(240);
    //         motor2.setSpeed(255);
    //     }
    // } else if (state.dy < 0) {
    //     if (state.dx == 0) {
    //         motor1.setSpeed(-255);
    //         motor2.setSpeed(-255);
    //     } else if (state.dx > 0) {
    //         motor1.setSpeed(-255);
    //         motor2.setSpeed(-240);
    //     } else if (state.dx < 0) {
    //         motor1.setSpeed(-240);
    //         motor2.setSpeed(-255);
    //     }
    // }
}

void Robot::updateServo() {
    updateCount();
    controller_state_t state = controller.getState();
    if (state.dy > 0) {
        servo1.setAngle(96);
        servo2.setAngle(86);
        wheelDown = false;
    } else if (state.dy < 0) {
        servo1.setAngle(86);
        servo2.setAngle(96);
        wheelDown = true;
    }
    if (state.x) {
        servo4.setAngle(75);
    } else if (state.a) {
        servo4.setAngle(90);
    }
    if (state.y) {
        servo6.setAngle(75);
    } else if (state.b) {
        servo6.setAngle(90);
    }
}

void Robot::updateCount() {
    controller_state_t state = controller.getState();
    if (state.start || state.back) {
        return;
    }
    bool updateCountLZ = false;
    bool updateCountRZ = false;
    if (state.lz > 0) {
        countLZ = min(countLZ + 1, 50);
        updateCountLZ = true;
    } else if (state.lz < 0) {
        countLZ = max(countLZ - 1, 0);
        updateCountLZ = true;
    }
    if (state.rz > 0) {
        countRZ = min(countRZ + 1, 50);
        updateCountRZ = true;
    } else if (state.rz < 0) {
        countRZ = max(countRZ - 1, 0);
        updateCountRZ = true;
    }
    if (updateCountLZ) {
        servo3.setAngle(180.0f - 180.0f / 50.0f * countLZ);
    }
    if (updateCountRZ) {
        servo5.setAngle(180.0f - 180.0f / 50.0f * countRZ);
    }
}

void Robot::needUpdateSpeed() {
    robot_update_t value = ROBOT_UPDATE_SPEED;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateServo() {
    robot_update_t value = ROBOT_UPDATE_SERVO;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateAutoStart() {
    robot_update_t value = ROBOT_UPDATE_START;
    xQueueSend(needQueue, &value, 0);
}

void Robot::needUpdateAutoStop() {
    robot_update_t value = ROBOT_UPDATE_STOP;
    xQueueSend(needQueue, &value, 0);
}

void Robot::stop() {
    setSpeed(0, 0, 0, 0);
}

void Robot::task() {
    motorLF.begin();
    motorRF.begin();
    motorLB.begin();
    motorRB.begin();
    motor1.begin();
    motor2.begin();
    motor3.begin();
    servo1.begin(96);
    servo2.begin(86);
    servo3.setMaxAngle(360);
    servo3.begin(180);
    servo4.begin();
    servo5.begin(180);
    servo6.begin();
    servo7.begin();
    servo8.begin();
#if ROBOT_HAS_LIDAR
    lidar.start();
#endif
    while (true) {
        robot_update_t update;
        if (xQueueReceive(needQueue, &update, 50)) {
            switch (update) {
                case ROBOT_UPDATE_STOP:
                    autoStop();
                    break;
                case ROBOT_UPDATE_START:
                    autoStart();
                    break;
                case ROBOT_UPDATE_SPEED:
                    updateSpeed();
                    break;
                case ROBOT_UPDATE_SERVO:
                    updateServo();
                    break;
            }
        } else {
            updateCount();
        }
    }
}

void Robot::task(void* arg) {
    robot.task();
}

void Robot::autoTask() {
    log_i("Robot: auto start");
    road_object_t objects[4];
    int objectCount = 0;
    int signWidth = -1;
    int signIndex = -1;
#if ROBOT_HAS_LIDAR
    for (int n = 0; n < 10; n++) {
        lidar.scanRoadObjects(objects, objectCount, 4);
        vTaskDelay(100);
    }
    for (int n = 0; n < objectCount; n++) {
        road_object_t object = objects[n];
        if (signWidth < object.width) {
            signWidth = object.width;
            signIndex = n;
        }
    }
    if (signIndex >= 0) {
        road_object_t object = objects[signIndex];
        log_i("Lidar: Sign: angle=%d-%d, distance=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
    }
    for (int n = 0; n < objectCount; n ++) {
        if (n != signIndex) {
            road_object_t object = objects[n];
            log_i("Lidar: Pillar: angle=%d-%d, distance=%d-%d (%d), width=%d", object.angle0, object.angle1, object.distance0, object.distance1, object.distance, object.width);
        }
    }
#endif
#if ROBOT_HAS_IMU
    int startAxisX = imu.getAxisX();
    int startAxisY = imu.getAxisY();
    int startAxisZ = imu.getAxisZ();
    log_i("Gyroscope: x=%d, y=%d, z=%d", startAxisX, startAxisY, startAxisZ);
#endif
    controller_state_t state = controller.getState();
    state.ly = 7;
    controller.setState(state);
    vTaskDelay(1000);
    autoEnd();
    autoStartedTask = nullptr;
    vTaskDelete(NULL);
}

void Robot::autoTask(void* arg) {
    robot.autoTask();
}
# 1 "servo.cpp"

#include "servo.h"

#if ROBOT_HAS_SERVO_1
ServoPWM servo1 = ServoPWM("1", ROBOT_SERVO_1_PIN);
#else
Servo servo1 = Servo("1");
#endif
#if ROBOT_HAS_SERVO_2
ServoPWM servo2 = ServoPWM("2", ROBOT_SERVO_2_PIN);
#else
Servo servo2 = Servo("2");
#endif
#if ROBOT_HAS_SERVO_3
ServoPWM servo3 = ServoPWM("3", ROBOT_SERVO_3_PIN);
#else
Servo servo3 = Servo("3");
#endif
#if ROBOT_HAS_SERVO_4
ServoPWM servo4 = ServoPWM("4", ROBOT_SERVO_4_PIN);
#else
Servo servo4 = Servo("4");
#endif
#if ROBOT_HAS_SERVO_5
ServoPWM servo5 = ServoPWM("5", ROBOT_SERVO_5_PIN);
#else
Servo servo5 = Servo("5");
#endif
#if ROBOT_HAS_SERVO_6
ServoPWM servo6 = ServoPWM("6", ROBOT_SERVO_6_PIN);
#else
Servo servo6 = Servo("6");
#endif
#if ROBOT_HAS_SERVO_7
ServoPWM servo7 = ServoPWM("7", ROBOT_SERVO_7_PIN);
#else
Servo servo7 = Servo("7");
#endif
#if ROBOT_HAS_SERVO_8
ServoPWM servo8 = ServoPWM("8", ROBOT_SERVO_8_PIN);
#else
Servo servo8 = Servo("8");
#endif

Servo::Servo(const char* _name) {
    name = _name;
}

Servo::~Servo() {}

void Servo::begin(float angle) {}

const char* Servo::getName() {
    return name;
}

void Servo::setMinAngle(float value) {
    minAngle = value;
}

void Servo::setMaxAngle(float value) {
    maxAngle = value;
}

void Servo::setAngle(float value, bool force) {
    float newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        log_i("Servo %s: %d", name, (int)angle);
    }
}

ServoPWM::ServoPWM(const char* _name, uint8_t _pwmPin) : Servo(_name) {
    pwmPin = _pwmPin;
}

ServoPWM::~ServoPWM() {}

void ServoPWM::begin(float angle) {
    if (pwmPin) {
        ledcAttach(pwmPin, 50, 12);
        setAngle(angle, true);
    }
}

void ServoPWM::setAngle(float value, bool force) {
    float newAngle = value;
    if (newAngle < minAngle) {
        newAngle = minAngle;
    } else if (newAngle > maxAngle) {
        newAngle = maxAngle;
    }
    if (angle != newAngle || force) {
        angle = newAngle;
        ledcWrite(pwmPin, map(newAngle, minAngle, maxAngle, 500, 2500) * 4096 / 20000);
        log_i("Servo %s: %d", name, (int)angle);
    }
}

# 1 "settings.cpp"

#include "settings.h"

Settings settings;

Settings::Settings() : Preferences() {}

Settings::~Settings() {}

void Settings::begin() {
    begin("robot");
    started = true;
    read();
}

void Settings::read() {
    if (isKey("ota.mode")) {
        otaMode = (ota_mode_t)getUChar("ota.mode");
    }
    if (isKey("wifi.mode")) {
        wifiMode = (wifi_mode_t)getUChar("wifi.mode");
    }
    if (isKey("wifi.ssid")) {
        wifiSSID = getString("wifi.ssid");
    }
    if (isKey("wifi.password")) {
        wifiPassword = getString("wifi.password");
    }
    if (isKey("robot.settings")) {
        robotSettings = getString("robot.settings");
    }
}

ota_mode_t Settings::getOtaMode() {
    return otaMode;
}

bool Settings::hasOtaBLE() {
    return otaMode & OTA_BLE;
}

bool Settings::hasOtaWiFi() {
    return otaMode & OTA_WIFI;
}

bool Settings::setOtaMode(ota_mode_t value) {
    otaMode = value;
    log_i("ota.mode: %d", value);
    return true;
}

bool Settings::addOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_BLE);
    }
}

bool Settings::addOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_WIFI);
    }
}

bool Settings::removeOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_WIFI);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

bool Settings::removeOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_BLE);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

wifi_mode_t Settings::getWiFiMode() {
    return wifiMode;
}

bool Settings::setWiFiMode(wifi_mode_t value) {
    wifiMode = value;
    log_i("wifi.mode: %d", value);
    putUChar("wifi.mode", value);
    return true;
}

String Settings::getWiFiSSID() {
    return wifiSSID;
}

bool Settings::setWiFiSSID(String value) {
    wifiSSID = value;
    log_i("wifi.ssid: %s", value.c_str());
    putString("wifi.ssid", value);
    return true;
}

String Settings::getWiFiPassword() {
    return wifiPassword;
}

bool Settings::setWiFiPassword(String value) {
    wifiPassword = value;
    log_i("wifi.password: %s", value.c_str());
    putString("wifi.password", value);
    return true;
}

String Settings::getRobotSettings() {
    return robotSettings;
}

bool Settings::setRobotSettings(String value) {
    robotSettings = value;
    log_i("robot.settings: %02x", value.charAt(0));
    putString("robot.settings", value);
    return true;
}

void Settings::end() {
    Preferences::end();
    started = false;
}

# 1 "tea.cpp"

#include "tea.h"

TEA tea;

TEA::TEA() {}

TEA::~TEA() {}

bool TEA::encryptData(uint8_t* data) {
    unsigned long salt = millis();
    data[7] = salt & 0xff;
    data[6] = (salt >> 8) & 0xff;
    data[5] = 0;
    uint32_t* block = (uint32_t*)data;
    encryptBlock(block);
    return true;
}

bool TEA::decryptData(uint8_t* data) {
    uint32_t* block = (uint32_t*)data;
    decryptBlock(block);
    return data[5] == 0;
}

void TEA::encryptBlock(uint32_t* block) {
    uint32_t b0 = block[0], b1 = block[1], sum = 0;
    for (uint32_t i = 0; i < count; i++) {
        sum += delta;
        b0 += ((b1 << 4) | key[0]) ^ (b1 | sum) ^ ((b1 >> 5) | key[1]);
        b1 += ((b0 << 4) | key[2]) ^ (b0 | sum) ^ ((b0 >> 5) | key[3]);
    }
    block[0] = b0;
    block[1] = b1;
}

void TEA::decryptBlock(uint32_t* block) {
    uint32_t b0 = block[0], b1 = block[1], sum = sumDelta;
    for (uint32_t i = 0; i < count; i++) {
        b1 -= ((b0 << 4) | key[2]) ^ (b0 | sum) ^ ((b0 >> 5) | key[3]);
        b0 -= ((b1 << 4) | key[0]) ^ (b1 | sum) ^ ((b1 >> 5) | key[1]);
        sum -= delta;
    }
    block[0] = b0;
    block[1] = b1;
}

# 1 "usb.cpp"

#if ROBOT_HAS_CONTROLLER_USB

#include "usb.h"

#include "controller.h"

USB usb;

USB::USB() {}

USB::~USB() {}

void USB::begin() {
    xTaskCreate(hidHostTask, "usb_hid_host_task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(hostLibTask, "usb_host_lib_task", 4096, xTaskGetCurrentTaskHandle(), 1, NULL, 0);
    ulTaskNotifyTake(false, 1000);
    const hid_host_driver_config_t driver_config = {
        .create_background_task = true,
        .task_priority = 1,
        .stack_size = 4096,
        .core_id = 0,
        .callback = hidHostDriverEventCallback,
        .callback_arg = NULL,
    };
    hid_host_install(&driver_config);
}

void USB::hostLibTask(void* arg) {
    const usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    usb_host_install(&host_config);
    xTaskNotifyGive((TaskHandle_t)arg);
    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
    }
    vTaskDelay(50);
    usb_host_uninstall();
    vTaskDelete(NULL);
}

QueueHandle_t USB::hidHostQueue = xQueueCreate(10, sizeof(usb_hid_host_device_event_t));

void USB::hidHostTask(void* arg) {
    usb_hid_host_device_event_t device_event;
    while (true) {
        if (xQueueReceive(hidHostQueue, &device_event, pdMS_TO_TICKS(50))) {
            hidHostDriverEventHandler(device_event.device_handle, device_event.driver_event, device_event.arg);
        }
    }
    xQueueReset(hidHostQueue);
    vTaskDelete(NULL);
}

void USB::hidHostDriverEventHandler(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg) {
    switch (driver_event) {
        case HID_HOST_DRIVER_EVENT_CONNECTED:
            log_i("USB HID: Connected");
            const hid_host_device_config_t device_config = {
                .callback = hidHostInterfaceEventCallback,
                .callback_arg = NULL,
            };
            hid_host_device_open(device_handle, &device_config);
            hid_host_device_start(device_handle);
            break;
    }
}

void USB::hidHostDriverEventCallback(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg) {
    const usb_hid_host_device_event_t device_event = {
        .device_handle = device_handle,
        .driver_event = driver_event,
        .arg = arg,
    };
    xQueueSend(hidHostQueue, &device_event, 0);
}

void USB::hidHostInterfaceEventCallback(hid_host_device_handle_t device_handle, const hid_host_interface_event_t interface_event, void* arg) {
    switch (interface_event) {
        case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
            hidHostInterfaceInputReportEventHandler(device_handle);
            break;
        case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
            log_i("USB HID: Disconnected");
            hid_host_device_close(device_handle);
            break;
        case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
            log_e("USB HID: Transfer error");
            break;
    }
}

void USB::hidHostInterfaceInputReportEventHandler(hid_host_device_handle_t device_handle) {
    const usb_device_desc_t* device_descriptor;
    hid_iface_t* hid_iface = (hid_iface_t*)device_handle;
    usb_host_get_device_descriptor(hid_iface->parent->dev_hdl, &device_descriptor);
    if (device_descriptor->idVendor == 0x046d) {
        if (device_descriptor->idProduct == 0xc219) {
            uint8_t data[8] = {0};
            size_t length = 0;
            hid_host_device_get_raw_input_report_data(device_handle, data, 8, &length);
            if (length == 8) {
                // log_i("data=0x%02x%02x%02x%02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
                usb_hid_controller_0219_state_t* state = (usb_hid_controller_0219_state_t*)data;
                controller_state_t controller_state = {
                    .lx = (int8_t)(state->lx < 0x70 ? -((0x7f - state->lx) >> 4) : (state->lx >= 0x90 ? (state->lx - 0x80) >> 4 : 0)),
                    .ly = (int8_t)(state->ly < 0x70 ? (0x7f - state->ly) >> 4 : (state->ly >= 0x90 ? -((state->ly - 0x80) >> 4) : 0)),
                    .rx = (int8_t)(state->rx < 0x70 ? -((0x7f - state->rx) >> 4) : (state->rx >= 0x90 ? (state->rx - 0x80) >> 4 : 0)),
                    .ry = (int8_t)(state->ry < 0x70 ? (0x7f - state->ry) >> 4 : (state->ry >= 0x90 ? -((state->ry - 0x80) >> 4) : 0)),
                    .dx = (int8_t)(state->dpad == 8 || state->dpad == 0 || state->dpad == 4 ? 0 : (state->dpad > 0 && state->dpad < 4 ? 1 : -1)),
                    .dy = (int8_t)(state->dpad == 8 || state->dpad == 2 || state->dpad == 6 ? 0 : (state->dpad < 2 || state->dpad > 6 ? 1 : -1)),
                    .lz = (int8_t)(state->lzd ? (state->lzu ? -2 : -1) : (state->lzu ? 1 : 0)),
                    .rz = (int8_t)(state->rzd ? (state->rzu ? -2 : -1) : (state->rzu ? 1 : 0)),
                    .lt = state->lt ? true : false,
                    .rt = state->rt ? true : false,
                    .a = state->a ? true : false,
                    .b = state->b ? true : false,
                    .x = state->x ? true : false,
                    .y = state->y ? true : false,
                    .start = state->start ? true : false,
                    .back = state->back ? true : false,
                    .mode = state->mode,
                };
                controller.setState(controller_state);
            }
        } else if (device_descriptor->idProduct == 0xc216) {
            uint8_t data[8] = {0};
            size_t length = 0;
            hid_host_device_get_raw_input_report_data(device_handle, data, 8, &length);
            if (length == 8) {
                // log_i("data=0x%02x%02x%02x%02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
                usb_hid_controller_0216_state_t* state = (usb_hid_controller_0216_state_t*)data;
                controller_state_t controller_state = {
                    .lx = (int8_t)(state->lx < 0x70 ? -((0x7f - state->lx) >> 4) : (state->lx >= 0x90 ? (state->lx - 0x80) >> 4 : 0)),
                    .ly = (int8_t)(state->ly < 0x70 ? (0x7f - state->ly) >> 4 : (state->ly >= 0x90 ? -((state->ly - 0x80) >> 4) : 0)),
                    .rx = (int8_t)(state->rx < 0x70 ? -((0x7f - state->rx) >> 4) : (state->rx >= 0x90 ? (state->rx - 0x80) >> 4 : 0)),
                    .ry = (int8_t)(state->ry < 0x70 ? (0x7f - state->ry) >> 4 : (state->ry >= 0x90 ? -((state->ry - 0x80) >> 4) : 0)),
                    .dx = (int8_t)(state->dpad == 8 || state->dpad == 0 || state->dpad == 4 ? 0 : (state->dpad > 0 && state->dpad < 4 ? 1 : -1)),
                    .dy = (int8_t)(state->dpad == 8 || state->dpad == 2 || state->dpad == 6 ? 0 : (state->dpad < 2 || state->dpad > 6 ? 1 : -1)),
                    .lz = (int8_t)(state->lzd ? (state->lzu ? -2 : -1) : (state->lzu ? 1 : 0)),
                    .rz = (int8_t)(state->rzd ? (state->rzu ? -2 : -1) : (state->rzu ? 1 : 0)),
                    .lt = state->lt ? true : false,
                    .rt = state->rt ? true : false,
                    .a = state->a ? true : false,
                    .b = state->b ? true : false,
                    .x = state->x ? true : false,
                    .y = state->y ? true : false,
                    .start = state->start ? true : false,
                    .back = state->back ? true : false,
                    .mode = state->mode,
                };
                controller.setState(controller_state);
            }
        }
    }
}

#endif

