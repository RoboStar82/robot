
#include "robot.h"

Robot *robot = new Robot;

Robot *getRobot() {
    return robot;
}

void robotSetup() {
    robot->setController(getController());
    robot->setSticks(new Stick, new Stick, new Stick);
#if ROBOT_HAS_MOTOR_I2C || ROBOT_HAS_MOTOR_PWM
    robot->setMotors(new Motor(1), new Motor(2), new Motor(3), new Motor(4));
#endif
#if ROBOT_HAS_SERVO_I2C
    robot->setServos(new Servo(1), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
#endif
#if ROBOT_HAS_SERVO_PWM
    robot->setServos(nullptr, new Servo(2), nullptr, nullptr, new Servo(5), nullptr, nullptr, nullptr);
#endif
    uint8_t *health = getHealth();
    int healthSize = getHealthSize();
    if (robot->motorLF != nullptr) {
        health[4] |= (robot->motorLF->health ? 1 : 0) << 0;
    }
    if (robot->motorRF != nullptr) {
        health[4] |= (robot->motorRF->health ? 1 : 0) << 1;
    }
    if (robot->motorLB != nullptr) {
        health[4] |= (robot->motorLB->health ? 1 : 0) << 2;
    }
    if (robot->motorRB != nullptr) {
        health[4] |= (robot->motorRB->health ? 1 : 0) << 3;
    }
    BLECharacteristic *healthCharacteristic = getHealthCharacteristic();
    healthCharacteristic->setValue(health, healthSize);
    xTaskCreatePinnedToCore(robotBegin, "robot", 4096, NULL, 1, NULL, 1);
}

void robotBegin(void *params) {
    while (true) {
        robot->loop();
    }
}

void robotLoop() {
    robot->loop();
}

// Робот

void Robot::setController(Controller *controller) {
    this->controller = controller;
};

void Robot::setSticks(Stick *L, Stick *R, Stick *D) {
    this->L = L;
    this->R = R;
    this->D = D;
}

void Robot::setMotors(Motor *motorLF, Motor *motorRF, Motor *motorLB, Motor *motorRB) {
    this->motorLF = motorLF;
    this->motorRF = motorRF;
    this->motorLB = motorLB;
    this->motorRB = motorRB;
    motors[0] = motorLF;
    motors[1] = motorRF;
    motors[2] = motorLB;
    motors[3] = motorRB;
}

void Robot::setServos(Servo *servo1, Servo *servo2, Servo *servo3, Servo *servo4, Servo *servo5, Servo *servo6, Servo *servo7, Servo *servo8) {
    this->servo1 = servo1;
    this->servo2 = servo2;
    this->servo3 = servo3;
    this->servo4 = servo4;
    this->servo5 = servo5;
    this->servo6 = servo6;
    this->servo7 = servo7;
    this->servo8 = servo8;
    servos[0] = servo1;
    servos[1] = servo2;
    servos[2] = servo3;
    servos[3] = servo4;
    servos[4] = servo5;
    servos[5] = servo6;
    servos[6] = servo7;
    servos[7] = servo8;
}

void Robot::updateSpeed() {
    int newSpeed = 0;
    int newSpeedLF = 0;
    int newSpeedRF = 0;
    int newSpeedLB = 0;
    int newSpeedRB = 0;
    int signX = 0;
    int signY = 0;
    int absX = 0;
    int absY = 0;

    Stick *L = nullptr;
    Stick *R = nullptr;

    if (this->L->x || this->L->y) {
        L = this->L;
    } else if (this->D->x || this->D->y) {
        L = this->D;
    } else if (this->controller->LT) {
        L = nullptr;
    } else {
        L = this->R;
    }

    if (this->R->x || this->R->y) {
        R = this->R;
    } else if (this->D->x || this->D->y) {
        R = this->D;
    } else if (this->controller->RT) {
        R = nullptr;
    } else {
        R = this->L;
    }

    // Левые моторы
    if (L == nullptr || L->x == 0 && L->y == 0) {
        // Остановка
        newSpeedLF = 0;
        newSpeedLB = 0;
    } else {
        signX = L->x > 0 ? 1 : (L->x < 0 ? -1 : 0);
        signY = L->y > 0 ? 1 : (L->y < 0 ? -1 : 0);
        absX = abs(L->x);
        absY = abs(L->y);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот влево: левые колёса едут медленнее
            // Поворот вправо: левые колёса без корректировки
            newSpeed = L->y + (signX >= 0 ? 0 : signY) * L->x;
            newSpeedLF = newSpeed;
            newSpeedLB = newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = L->x - (signY > 0 ? signX : 0) * L->y;
            newSpeedLF = -newSpeed;
            newSpeed = L->x + (signY < 0 ? signX : 0) * L->y;
            newSpeedLB = newSpeed;
        }
    }

    // Правые моторы
    if (R == nullptr || R->x == 0 && R->y == 0) {
        // Остановка
        newSpeedRF = 0;
        newSpeedRB = 0;
    } else {
        signX = R->x > 0 ? 1 : (R->x < 0 ? -1 : 0);
        signY = R->y > 0 ? 1 : (R->y < 0 ? -1 : 0);
        absX = abs(R->x);
        absY = abs(R->y);
        if (absX <= absY) {
            // Движение вперед-назад
            // Поворот вправо: правые колёса едут медленнее
            // Поворот влево: правые колёса без корректировки
            newSpeed = R->y - (signX <= 0 ? 0 : signY) * R->x;
            newSpeedRF = newSpeed;
            newSpeedRB = newSpeed;
        } else {
            // Движение вправо-влево
            // Поворот вверх: верхнее колесо едет медленнее; нижнее колесо без корректировки
            // Поворот вниз: нижнее колесо едет медленнее; верхнее колесо без корректировки
            newSpeed = R->x - (signY > 0 ? signX : 0) * R->y;
            newSpeedRF = newSpeed;
            newSpeed = R->x + (signY < 0 ? signX : 0) * R->y;
            newSpeedRB = -newSpeed;
        }
    }

    if (this->controller->LT || this->controller->RT) {
        newSpeedLF = (float)newSpeedLF / 4;
        newSpeedRF = (float)newSpeedRF / 4;
        newSpeedLB = (float)newSpeedLB / 4;
        newSpeedRB = (float)newSpeedRB / 4;
    }

    if (motorLF->speed != newSpeedLF) {
#if DEBUG_MOTOR
        debug("V: motor: LF: %d\n", newSpeedLF);
#endif
        motorLF->setSpeed(newSpeedLF);
#if ROBOT_HAS_LED
        uint32_t color;
        if (newSpeedLF == 0) {
            color = 0x000000;
        } else if (newSpeedLF > 0) {
            color = (newSpeedLF & 0xff) | ((newSpeedLF & 0xff) << 8) | ((newSpeedLF & 0xff) << 16);
        } else {
            color = ((-newSpeedLF) & 0xff) << 16;
        }
        led->setPixelColor(2, color);
#endif
    }
    if (motorRF->speed != newSpeedRF) {
#if DEBUG_MOTOR
        debug("V: motor: RF: %d\n", newSpeedRF);
#endif
        motorRF->setSpeed(newSpeedRF);
#if ROBOT_HAS_LED
        uint32_t color;
        if (newSpeedRF == 0) {
            color = 0x000000;
        } else if (newSpeedRF > 0) {
            color = (newSpeedRF & 0xff) | ((newSpeedRF & 0xff) << 8) | ((newSpeedRF & 0xff) << 16);
        } else {
            color = ((-newSpeedRF) & 0xff) << 16;
        }
        led->setPixelColor(1, color);
#endif
    }
    if (motorLB->speed != newSpeedLB) {
#if DEBUG_MOTOR
        debug("V: motor: LB: %d\n", newSpeedLB);
#endif
        motorLB->setSpeed(newSpeedLB);
#if ROBOT_HAS_LED
        uint32_t color;
        if (newSpeedLB == 0) {
            color = 0x000000;
        } else if (newSpeedLB > 0) {
            color = (newSpeedLB & 0xff) | ((newSpeedLB & 0xff) << 8) | ((newSpeedLB & 0xff) << 16);
        } else {
            color = ((-newSpeedLB) & 0xff) << 16;
        }
        led->setPixelColor(3, color);
#endif
    }
    if (motorRB->speed != newSpeedRB) {
#if DEBUG_MOTOR
        debug("V: motor: RB: %d\n", newSpeedRB);
#endif
        motorRB->setSpeed(newSpeedRB);
#if ROBOT_HAS_LED
        uint32_t color;
        if (newSpeedRB == 0) {
            color = 0x000000;
        } else if (newSpeedRB > 0) {
            color = (newSpeedRB & 0xff) | ((newSpeedRB & 0xff) << 8) | ((newSpeedRB & 0xff) << 16);
        } else {
            color = ((-newSpeedRB) & 0xff) << 16;
        }
        led->setPixelColor(0, color);
#endif
    }
#if ROBOT_HAS_LED
    led->show();
#endif
}

void Robot::loop() {
    if (changeXY) {
        changeXY = false;
        if (L->getNewX() != controller->LX) {
            L->setNewX(controller->LX);
        }
        if (L->getNewY() != controller->LY) {
            L->setNewY(controller->LY);
        }
        if (R->getNewX() != controller->RX) {
            R->setNewX(controller->RX);
        }
        if (R->getNewY() != controller->RY) {
            R->setNewY(controller->RY);
        }
        if (D->getNewX() != controller->DX) {
            D->setNewX(controller->DX);
        }
        if (D->getNewY() != controller->DY) {
            D->setNewY(controller->DY);
        }
    }
    bool update = false;
    if (L->changeX || L->changeY) {
        L->loop();
        update = true;
    }
    if (R->changeX || R->changeY) {
        R->loop();
        update = true;
    }
    if (D->changeX || D->changeY) {
        D->loop();
        update = true;
    }
    if (update) {
        updateSpeed();
    }
    delay(50);
}

// Обработка изменения настроек

void SettingsCharacteristicCallbacks::onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo &connInfo) {
    std::string value = bleCharacteristic->getValue();
    if (value.length() >= 4) {
        int minMotorSpeed = value[0];
        int maxMotorSpeed = value[1];
        int stickDamper = value[2];
        int servoSpeed = value[3];
        int servoSpeed2 = servoSpeed;
        if (value.length() >= 5) {
            servoSpeed2 = value[4];
        }
#if DEBUG_SETTINGS
        debug("V: settings: minMotorSpeed = %d\n", minMotorSpeed);
        debug("V: settings: maxMotorSpeed = %d\n", maxMotorSpeed);
        debug("V: settings: stickDamper = %d\n", stickDamper);
        debug("V: settings: servoSpeed = %d\n", servoSpeed);
        if (value.length() >= 5) {
            debug("V: settings: servoSpeed2 = %d\n", servoSpeed2);
        }
#endif
        if (robot->motorLF != nullptr) {
            robot->motorLF->setMinSpeed(minMotorSpeed);
            robot->motorLF->setMaxSpeed(maxMotorSpeed);
        }
        if (robot->motorRF != nullptr) {
            robot->motorRF->setMinSpeed(minMotorSpeed);
            robot->motorRF->setMaxSpeed(maxMotorSpeed);
        }
        if (robot->motorLB != nullptr) {
            robot->motorLB->setMinSpeed(minMotorSpeed);
            robot->motorLB->setMaxSpeed(maxMotorSpeed);
        }
        if (robot->motorRB != nullptr) {
            robot->motorRB->setMinSpeed(minMotorSpeed);
            robot->motorRB->setMaxSpeed(maxMotorSpeed);
        }
        if (robot->L != nullptr) {
            robot->L->setDamper(stickDamper);
        }
        if (robot->R != nullptr) {
            robot->R->setDamper(stickDamper);
        }
        if (robot->D != nullptr) {
            robot->D->setDamper(stickDamper);
        }
        if (robot->servo1 != nullptr) {
            robot->servo1->setMaxSpeed(servoSpeed);
        }
        if (robot->servo2 != nullptr) {
            robot->servo2->setMaxSpeed(servoSpeed2);
        }
        if (robot->servo3 != nullptr) {
            robot->servo3->setMaxSpeed(servoSpeed);
        }
        if (robot->servo4 != nullptr) {
            robot->servo4->setMaxSpeed(servoSpeed);
        }
    }
}

// Обработка изменения состояний кнопок

void Controller::onChangeStart() {
#if DEBUG_CONTROL
    println("V: robot: start");
#endif
#if ROBOT_HAS_LIDAR
    lidarStart();
#endif
}

void Controller::onChangeBack() {
#if DEBUG_CONTROL
    println("V: robot: back");
#endif
    if (robot->autoMode) {
        robot->autoMode = 0;
    } else {
#if ROBOT_HAS_LIDAR
        lidarStop();
#endif
    }
}

void Controller::onChangeA() {
#if DEBUG_CONTROL
    println("V: robot: A");
#endif
    if (robot->servo5 != nullptr) {
        robot->servo5->setAngle(0);
    } else {
        robot->autoMode = 'A';
    }
}

void Controller::onChangeB() {
#if DEBUG_CONTROL
    println("V: robot: B");
#endif
    if (robot->servo5 != nullptr) {
        robot->servo5->setAngle(30);
    } else {
        robot->autoMode = 'B';
    }
}

void Controller::onChangeX() {
#if DEBUG_CONTROL
    println("V: robot: X");
#endif
    if (robot->servo5 != nullptr) {
        robot->servo5->setAngle(60);
    } else {
        robot->autoMode = 'X';
    }
}

void Controller::onChangeY() {
#if DEBUG_CONTROL
    println("V: robot: Y");
#endif
    if (robot->servo5 != nullptr) {
        robot->servo5->setAngle(90);
    } else {
        robot->autoMode = 'Y';
    }
}

void Controller::onChangeZ() {
#if DEBUG_CONTROL
    debug("V: robot: Z: %d %d\n", robot->controller->LZ, robot->controller->RZ);
#endif
    if (robot->servo1 != nullptr) {
        robot->servo1->setSpeed(robot->controller->LZ);
    }
    if (robot->servo2 != nullptr) {
        robot->servo2->setSpeed(robot->controller->RZ);
    }
#if ROBOT_HAS_LED
#if ROBOT_HAS_SERVO_PWM
    uint32_t color;

    if (robot->controller->RZ == 0) {
        color = 0x000000;
    } else if (robot->controller->RZ > 0) {
        color = (0xff) | ((0xff) << 8) | ((0xff) << 16);
    } else {
        color = (0xff) << 16;
    }
    robot->led->setPixelColor(0, color);
    robot->led->setPixelColor(1, color);
    robot->led->setPixelColor(2, color);
    robot->led->setPixelColor(3, color);

    robot->led->show();
#endif
#endif
}

void Controller::onChangeXY() {
    robot->changeXY = true;
    robot->autoMode = 0;
}

#if ROBOT_HAS_LED

int otaLoopN = 0;
int otaLoopB = 0;
bool otaLoop2 = false;

void otaBeginLed(void *params) {
    while (true) {
        otaLoopLed();
    }
}

void otaLoopLed() {
    if (otaLoop2) {
        otaLoopB--;
    } else {
        otaLoopB++;
    }
    uint32_t color = (0x00 << 16) | ((otaLoopB * 4) << 8) | ((otaLoopB * 4) << 0);
    robot->led->setPixelColor(otaLoopN, color);
    robot->led->show();
    delay(10);
    if (otaLoopB == 0) {
        otaLoopN = (otaLoopN + 1) % 4;
        otaLoop2 = !otaLoop2;
    } else if (otaLoopB == 32) {
        otaLoop2 = !otaLoop2;
    }
}

void otaEndLed() {
    robot->led->setPixelColor(0, 0);
    robot->led->setPixelColor(1, 0);
    robot->led->setPixelColor(2, 0);
    robot->led->setPixelColor(3, 0);
    robot->led->show();
}

#endif
