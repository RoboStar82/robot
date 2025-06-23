
#include <Wire.h>
#include <iarduino_I2C_Motor.h>

#include "lidar.h"
#include "bmx.h"
#include "ble.h"

// Порты моторов

#define MOTOR_LF_PORT 0x41
#define MOTOR_RF_PORT 0x42
#define MOTOR_LB_PORT 0x43
#define MOTOR_RB_PORT 0x44

// Параметры моторов

#define MOTOR_STOP_NEUTRAL true
#define MOTOR_NOMINAL_RPM 170
#define MOTOR_VOLTAGE 12.0f
#define MOTOR_REDUCER 53.0f
#define MOTOR_MAGNET 7

#define MOTOR_MIN_SPEED 20.0f
#define MOTOR_MAX_SPEED 80.0f
#define MOTOR_AVG_SPEED 60.0f

// Моторы

iarduino_I2C_Motor motorLF(MOTOR_LF_PORT);
iarduino_I2C_Motor motorRF(MOTOR_RF_PORT);
iarduino_I2C_Motor motorLB(MOTOR_LB_PORT);
iarduino_I2C_Motor motorRB(MOTOR_RB_PORT);

// Включена ли отладка

bool debugCtrl = true;
bool debugMotor = true;
bool debugPosition = false;
bool debugPositionTx = false;

// Кнопки

// Start
bool ctrlStart = false;
// Back
bool ctrlBack = false;

// A
bool ctrlA = false;
// B
bool ctrlB = false;
// X
bool ctrlX = false;
// Y
bool ctrlY = false;

// D-pad X (right-left)
int ctrlDX = 0;
// D-pad Y (up-down)
int ctrlDY = 0;

// D-pad average
int ctrlDA = round(((MOTOR_AVG_SPEED - MOTOR_MIN_SPEED) / (MOTOR_MAX_SPEED - MOTOR_MIN_SPEED)) * 7.0f);

// Left X (right-left)
int ctrlLX = 0;
// Left Y (up-down)
int ctrlLY = 0;
// Right X (right-left)
int ctrlRX = 0;
// Right Y (up-down)
int ctrlRY = 0;
// Left Z (up-down)
int ctrlLZ = 0;
// Right Z (up-down)
int ctrlRZ = 0;

// Скорости моторов

// Left front
int speedLF = 0;
// Right front
int speedRF = 0;
// Left back
int speedLB = 0;
// Right back
int speedRB = 0;

// Состояние робота

uint8_t robotHealth[1] = { 0 };

// Позиция робота

uint8_t robotPosition[512] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// Обработка изменения состояний кнопок

void changeStart() {
    if (debugCtrl) {
        Serial.print("V: control: start: ");
        Serial.println(ctrlStart);
    }
}

void changeBack() {
    if (debugCtrl) {
        Serial.print("V: control: back: ");
        Serial.println(ctrlBack);
    }
    if (ctrlBack) {
        ctrlLX = 0;
        ctrlLY = 0;
        ctrlRX = 0;
        ctrlRY = 0;
        speedLF = 0;
        speedRF = 0;
        speedLB = 0;
        speedRB = 0;
        setMotorSpeed(motorLF, 0);
        setMotorSpeed(motorRF, 0);
        setMotorSpeed(motorLB, 0);
        setMotorSpeed(motorRB, 0);
    }
}

void changeA() {
    if (debugCtrl) {
        Serial.print("V: control: A: ");
        Serial.println(ctrlA);
    }
}

void changeB() {
    if (debugCtrl) {
        Serial.print("V: control: B: ");
        Serial.println(ctrlB);
    }
}

void changeX() {
    if (debugCtrl) {
        Serial.print("V: control: X: ");
        Serial.println(ctrlX);
    }
}

void changeY() {
    if (debugCtrl) {
        Serial.print("V: control: Y: ");
        Serial.println(ctrlY);
    }
}

// Обработка изменения скоростей моторов

void setMotorSpeed(iarduino_I2C_Motor &motor, int speed) {
    if (speed == 0) {
        motor.setSpeed(0, MOT_RPM);
    } else if (speed > 0) {
        motor.setSpeed(MOTOR_MIN_SPEED + (MOTOR_MAX_SPEED - MOTOR_MIN_SPEED) * speed / 7 , MOT_RPM);
    } else if (speed < 0) {
        motor.setSpeed(-MOTOR_MIN_SPEED + (MOTOR_MAX_SPEED - MOTOR_MIN_SPEED) * speed / 7, MOT_RPM);
    } else {
        motor.setSpeed(0, MOT_RPM);
    }
}

void updateSpeed() {
    int newSpeedLF = 0;
    int newSpeedRF = 0;
    int newSpeedLB = 0;
    int newSpeedRB = 0;
    int absX;
    int absY;
    absX = abs(ctrlLX);
    absY = abs(ctrlLY);
    if (absX == 0 && absY == 0) {
        if (ctrlDX == 0 && ctrlDY == 0) {
            // Остановка
            newSpeedLF = 0;
            newSpeedLB = 0;
        } else if (ctrlDY != 0) {
            if (ctrlDX == 0) {
                // Движение вперед-назад
                newSpeedLF = ctrlDY < 0 ? -ctrlDA : ctrlDA;
                newSpeedLB = ctrlDY < 0 ? -ctrlDA : ctrlDA;
            } else {
                // Движение вперед-назад
                newSpeedLF = ctrlDY < 0 ? -ctrlDA : ctrlDA;
                newSpeedLB = ctrlDY < 0 ? -ctrlDA : ctrlDA;
            }
        } else if (ctrlDX != 0) {
            // Движение вправо-влево
            newSpeedLF = ctrlDX < 0 ? ctrlDA : -ctrlDA;
            newSpeedLB = ctrlDX < 0 ? -ctrlDA : ctrlDA;
        } else {
            // Остановка
            newSpeedLF = 0;
            newSpeedLB = 0;
        }
    } else if (absX <= absY) {
        // Движение вперед-назад
        newSpeedLF = ctrlLY;
        newSpeedLB = ctrlLY;
    } else {
        // Движение вправо-влево
        newSpeedLF = -ctrlLX;
        newSpeedLB = ctrlLX;
    }
    absX = abs(ctrlRX);
    absY = abs(ctrlRY);
    if (absX == 0 && absY == 0) {
        if (ctrlDX == 0 && ctrlDY == 0) {
            // Остановка
            newSpeedRF = 0;
            newSpeedRB = 0;
        } else if (ctrlDY != 0) {
            if (ctrlDX == 0) {
                // Движение вперед-назад
                newSpeedRF = ctrlDY < 0 ? -ctrlDA : ctrlDA;
                newSpeedRB = ctrlDY < 0 ? -ctrlDA : ctrlDA;
            } else {
                // Движение вперед-назад
                newSpeedRF = ctrlDY < 0 ? -ctrlDA : ctrlDA;
                newSpeedRB = ctrlDY < 0 ? -ctrlDA : ctrlDA;
            }
        } else if (ctrlDX != 0) {
            // Движение вправо-влево
            newSpeedRF = ctrlDX < 0 ? -ctrlDA : ctrlDA;
            newSpeedRB = ctrlDX < 0 ? ctrlDA : -ctrlDA;
        } else {
            // Остановка
            newSpeedRF = 0;
            newSpeedRB = 0;
        }
    } else if (absX <= absY) {
        // Движение вперед-назад
        newSpeedRF = ctrlRY;
        newSpeedRB = ctrlRY;
    } else {
        // Движение вправо-влево
        newSpeedRF = ctrlRX;
        newSpeedRB = -ctrlRX;
    }
    if (speedLF != newSpeedLF) {
        speedLF = newSpeedLF;
        if (debugMotor) {
            Serial.print("V: motor: LF: ");
            Serial.println(speedLF);
        }
        setMotorSpeed(motorLF, speedLF);
    }
    if (speedRF != newSpeedRF) {
        speedRF = newSpeedRF;
        if (debugMotor) {
            Serial.print("V: motor: RF: ");
            Serial.println(speedRF);
        }
        setMotorSpeed(motorRF, speedRF);
    }
    if (speedLB != newSpeedLB) {
        speedLB = newSpeedLB;
        if (debugMotor) {
            Serial.print("V: motor: LB: ");
            Serial.println(speedLB);
        }
        setMotorSpeed(motorLB, speedLB);
    }
    if (speedRB != newSpeedRB) {
        speedRB = newSpeedRB;
        if (debugMotor) {
            Serial.print("V: motor: RB: ");
            Serial.println(speedRB);
        }
        setMotorSpeed(motorRB, speedRB);
    }
}

// Подготовка моторов

bool motorSetup(iarduino_I2C_Motor &motor, bool clockwise) {
    bool r = motor.begin();
    motor.setMagnet(MOTOR_MAGNET);
    motor.setDirection(clockwise);
    motor.setStopNeutral(MOTOR_STOP_NEUTRAL);
    motor.setNominalRPM(MOTOR_NOMINAL_RPM);
    motor.setReducer(MOTOR_REDUCER);
    motor.setVoltage(MOTOR_VOLTAGE);
    return r;
}

void ServerCallbacks::onConnect(BLEServer* bleServer) {
    Serial.println("V: BLE: Connected");
}

void ServerCallbacks::onDisconnect(BLEServer* bleServer) {
    Serial.println("V: BLE: Disconnected");
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->start();
}

void CharacteristicCallbacks::onRobotControl(String value) {
    if (value.length() >= 4) {
        uint8_t btnValue = value[0];
        uint8_t padValue = value[1];
        uint8_t leftValue = value[2];
        uint8_t rightValue = value[3];
        bool newStart = (btnValue >> 0) & 1;
        bool newBack = (btnValue >> 1) & 1;
        bool newA = (btnValue >> 2) & 1;
        bool newB = (btnValue >> 3) & 1;
        bool newX = (btnValue >> 4) & 1;
        bool newY = (btnValue >> 5) & 1;
        int newDX = ((padValue >> 0) & 1) ? 1 : (((padValue >> 1) & 1) ? -1 : 0);
        int newDY = ((padValue >> 2) & 1) ? 1 : (((padValue >> 3) & 1) ? -1 : 0);
        int newLZ = ((padValue >> 4) & 1) ? 1 : (((padValue >> 5) & 1) ? -1 : 0);
        int newRZ = ((padValue >> 6) & 1) ? 1 : (((padValue >> 7) & 1) ? -1 : 0);
        uint8_t tmpValue;
        tmpValue = (leftValue >> 0) & 0xf;
        int newLX = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (leftValue >> 4) & 0xf;
        int newLY = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (rightValue >> 0) & 0xf;
        int newRX = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (rightValue >> 4) & 0xf;
        int newRY = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        if (ctrlStart != newStart) {
            ctrlStart = newStart;
            changeStart();
        }
        if (ctrlBack != newBack) {
            ctrlBack = newBack;
            changeBack();
        }
        if (ctrlA != newA) {
            ctrlA = newA;
            changeA();
        }
        if (ctrlB != newB) {
            ctrlB = newB;
            changeB();
        }
        if (ctrlX != newX) {
            ctrlX = newX;
            changeX();
        }
        if (ctrlY != newY) {
            ctrlY = newY;
            changeY();
        }
        if (false
            || (ctrlLX != newLX || ctrlLY != newLY)
            || (ctrlRX != newRX || ctrlRY != newRY)
            || (ctrlDX != newDX || ctrlDY != newDY)
        ) {
            ctrlLX = newLX;
            ctrlLY = newLY;
            ctrlRX = newRX;
            ctrlRY = newRY;
            ctrlDX = newDX;
            ctrlDY = newDY;
            updateSpeed();
        }
        if (ctrlLZ != newLZ) {
            ctrlLZ = newLZ;
        }
        if (ctrlRZ != newRZ) {
            ctrlRZ = newRZ;
        }
    }
}

void setup() {
    Serial.begin(115200);

    bleSetup();

    // Подготовка моторов
    robotHealth[0] |= (motorSetup(motorLF, true) ? 1 : 0) << 0;
    robotHealth[0] |= (motorSetup(motorRF, false) ? 1 : 0) << 1;
    robotHealth[0] |= (motorSetup(motorLB, true) ? 1 : 0) << 2;
    robotHealth[0] |= (motorSetup(motorRB, false) ? 1 : 0) << 3;

    BLECharacteristic *robotHealthCharacteristic = getRobotHealthCharacteristic();
    robotHealthCharacteristic->setValue(robotHealth, 1);
    robotHealthCharacteristic->notify();

    lidarSetup();
    bmxSetup();
}

void loop() {

    Lidar *lidar = getLidar();
    int length = 0;
    lidar->copyLoop(robotPosition + 2, length);

    iarduino_Position_BMX055 *sensor = getBMX();
    int angle = (360 + (int)sensor->axisZ) % 360;
    robotPosition[0] = angle & 0xff;
    length ++;
    robotPosition[1] = angle >> 8;
    length ++;

    if (debugPosition) {
        Serial.printf("V: BMX: angle: %d\n", angle);
        Serial.printf("V: lidar: angle: %d\n", robotPosition[2]);
    }

    if (debugPositionTx) {
        Serial.printf("V: BLE: position: %d:", length);
        for (int n = 0; n < length; n ++) {
            Serial.printf(" %02x", robotPosition[n]);
        }
        Serial.println();
    }

    BLECharacteristic *robotPositionCharacteristic = getRobotPositionCharacteristic();
    robotPositionCharacteristic->setValue(robotPosition, length);
    robotPositionCharacteristic->notify();

    delay(1000);
}
