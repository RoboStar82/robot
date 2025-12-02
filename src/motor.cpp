
#include "motor.h"

#if ROBOT_HAS_MOTOR_MCPWM
MotorMCPWM motorLF = MotorMCPWM("LF", 1, 2);
MotorMCPWM motorRF = MotorMCPWM("RF", 5, 4);
MotorMCPWM motorLB = MotorMCPWM("LB", 42, 41);
MotorMCPWM motorRB = MotorMCPWM("RB", 6, 7);
#if ROBOT_HAS_MOTOR_PWM
MotorPWM motorCC = MotorPWM("CC", 45, 47);
#else
MotorPWM motorCC = MotorMCPWM("CC", 45, 47);
#endif
#elif ROBOT_HAS_MOTOR_PWM
MotorPWM motorLF = MotorPWM("LF", 1, 2);
MotorPWM motorRF = MotorPWM("RF", 5, 4);
MotorPWM motorLB = MotorPWM("LB", 42, 41);
MotorPWM motorRB = MotorPWM("RB", 6, 7);
MotorPWM motorCC = MotorPWM("CC", 45, 47);
#elif ROBOT_HAS_MOTOR_ENCODER
MotorEncoder motorLF = MotorEncoder("LF", 1, 2);
MotorEncoder motorRF = MotorEncoder("RF", 5, 4);
MotorEncoder motorLB = MotorEncoder("LB", 42, 41);
MotorEncoder motorRB = MotorEncoder("RB", 6, 7);
Motor motorCC = Motor("CC");
#else
Motor motorLF = Motor("LF");
Motor motorRF = Motor("RF");
Motor motorLB = Motor("LB");
Motor motorRB = Motor("RB");
Motor motorCC = Motor("CC");
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

MotorEncoder::MotorEncoder(const char* _name, uint8_t _encoderPin1, uint8_t _encoderPin2) : Motor(_name) {
    encoderPin1 = _encoderPin1;
    encoderPin2 = _encoderPin2;
}

MotorEncoder::~MotorEncoder() {}

void MotorEncoder::begin() {}

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
            ledcWrite(pwmPin1, back ? absSpeed : 0);
            ledcWrite(pwmPin2, back ? 0 : absSpeed);
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
    maxSpeed = 1.0f * timerConfig.period_ticks;
    minSpeed = 0.5f * timerConfig.period_ticks;
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
}

void MotorMCPWM::setSpeed(int value) {
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
        if (back != (speed < 0)) {
            mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator1 : mcpwmGenerator2, back ? 0 : -1, true);
            mcpwm_generator_set_force_level(isLeft ? mcpwmGenerator2 : mcpwmGenerator1, back ? -1 : 0, true);
        }
        speed = newSpeed;
        mcpwm_comparator_set_compare_value(mcpwmComparator1, absSpeed);
        mcpwm_comparator_set_compare_value(mcpwmComparator2, absSpeed);
        log_i("Motor %s: %d", name, speed);
    }
}
