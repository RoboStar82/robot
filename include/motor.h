
#pragma once

#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include <driver/mcpwm_prelude.h>
#endif

class Motor {
   public:
    Motor(const char* name);
    virtual ~Motor() = default;

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
    virtual ~MotorPWM() = default;

    void begin();

    using Motor::getName;

    using Motor::getEncoderSpeed;
    using Motor::setEncoderSpeed;

    using Motor::setMaxSpeed;
    using Motor::setMinSpeed;

    void setSpeed(int value);

   protected:
    using Motor::encoderSpeed;
    using Motor::encoderWorks;
    using Motor::increaseSpeed;
    using Motor::maxSpeed;
    using Motor::minSpeed;
    using Motor::name;
    using Motor::speed;
    uint8_t pwmPin1 = 0;
    uint8_t pwmPin2 = 0;
};

#ifdef ARDUINO_ARCH_ESP32
class MotorMCPWM : MotorPWM {
   public:
    MotorMCPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);
    virtual ~MotorMCPWM() = default;

    void begin();

    using MotorPWM::getName;

    using MotorPWM::getEncoderSpeed;
    using MotorPWM::setEncoderSpeed;

    using MotorPWM::setMaxSpeed;
    using MotorPWM::setMinSpeed;

    void setSpeed(int value);

   protected:
    using MotorPWM::encoderSpeed;
    using MotorPWM::encoderWorks;
    using MotorPWM::increaseSpeed;
    using MotorPWM::maxSpeed;
    using MotorPWM::minSpeed;
    using MotorPWM::name;
    using MotorPWM::pwmPin1;
    using MotorPWM::pwmPin2;
    using MotorPWM::speed;

    bool isLeft = false;
    bool isRight = false;
    bool isFront = false;
    bool isBack = false;

    mcpwm_timer_handle_t mcpwmTimer = nullptr;
    mcpwm_oper_handle_t mcpwmOperator = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator1 = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator2 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator1 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator2 = nullptr;
};
#endif

#ifdef ROBOT_HAS_MOTOR_MCPWM
extern MotorMCPWM motorLF;
extern MotorMCPWM motorRF;
extern MotorMCPWM motorLB;
extern MotorMCPWM motorRB;
#else
#ifdef ROBOT_HAS_MOTOR_PWM
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
#endif
