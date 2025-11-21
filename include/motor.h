
#pragma once

#include <Arduino.h>
#include <driver/mcpwm_prelude.h>

class Motor {
   public:
    Motor(const char* name);

    void begin();

    const char* getName();

    void setMinSpeed(uint value);
    void setMaxSpeed(uint value);

    void setSpeed(int value);

   protected:
    const char* name;
    uint minSpeed = 50;
    uint maxSpeed = 200;
    int speed = 0;
};

class MotorEncoder : Motor {
   public:
    MotorEncoder(const char* name, uint8_t encoderPin1, uint8_t encoderPin2);

    void begin();

    using Motor::getName;

    using Motor::setMaxSpeed;
    using Motor::setMinSpeed;
    using Motor::setSpeed;

   protected:
    using Motor::maxSpeed;
    using Motor::minSpeed;
    using Motor::name;
    using Motor::speed;
    uint8_t encoderPin1 = 0;
    uint8_t encoderPin2 = 0;
};

class MotorPWM : Motor {
   public:
    MotorPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);

    void begin();

    using Motor::getName;

    using Motor::setMaxSpeed;
    using Motor::setMinSpeed;

    void setSpeed(int value);

   protected:
    using Motor::maxSpeed;
    using Motor::minSpeed;
    using Motor::name;
    using Motor::speed;
    uint8_t pwmPin1 = 0;
    uint8_t pwmPin2 = 0;
};

class MotorMCPWM : MotorPWM {
   public:
    MotorMCPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);

    void begin();

    using MotorPWM::getName;

    using MotorPWM::setMaxSpeed;
    using MotorPWM::setMinSpeed;

    void setSpeed(int value);

   protected:
    using MotorPWM::maxSpeed;
    using MotorPWM::minSpeed;
    using MotorPWM::name;
    using MotorPWM::pwmPin1;
    using MotorPWM::pwmPin2;
    using MotorPWM::speed;
    mcpwm_timer_handle_t mcpwmTimer = nullptr;
    mcpwm_oper_handle_t mcpwmOperator = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator1 = nullptr;
    mcpwm_cmpr_handle_t mcpwmComparator2 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator1 = nullptr;
    mcpwm_gen_handle_t mcpwmGenerator2 = nullptr;
};

#if ROBOT_HAS_MOTOR_PWM
extern MotorPWM motorLF;
extern MotorPWM motorRF;
extern MotorPWM motorLB;
extern MotorPWM motorRB;
extern MotorPWM motorCC;
#elif ROBOT_HAS_MOTOR_MCPWM
extern MotorMCPWM motorLF;
extern MotorMCPWM motorRF;
extern MotorMCPWM motorLB;
extern MotorMCPWM motorRB;
extern MotorMCPWM motorCC;
#elif ROBOT_HAS_MOTOR_ENCODER
extern MotorEncoder motorLF;
extern MotorEncoder motorRF;
extern MotorEncoder motorLB;
extern MotorEncoder motorRB;
extern Motor motorCC;
#else
extern Motor motorLF;
extern Motor motorRF;
extern Motor motorLB;
extern Motor motorRB;
extern Motor motorCC;
#endif
