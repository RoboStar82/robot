
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
